/* GStreamer
 * Copyright (C) 2021 Alex Pitrolo <alexpitrolo95@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Suite 500,
 * Boston, MA 02110-1335, USA.
 */

/**
 * SECTION:element-nvmaxinevideofx
 *
 * Gstreamer plugin that allows use of NVIDIA Maxine videofx sdk in a generic pipeline.
 * Supported effects:
 * - ArtifactReduction
 * - Upscale
 * - SuperRes
 * - Denoising
 * - GreenScreen
 * - BackgroundBlur
 * - Composition
 *
 * <refsect2>
 * <title>Example launch line</title>
 * |[
 * gst-launch-1.0 filesrc location="example_input.mp4" ! \
 * qtdemux ! avdec_h264 ! video/x-raw,width=example_width,height=example_height ! queue ! \
 * nvmaxinevideofx effect=SuperRes modeldir="/usr/local/VideoFx/lib/models" strength=1 upscalefactor=2 ! queue ! \
 * x264enc ! mp4mux ! filesink location="example_output.mp4"
 * ]|
 * A pipeline to demonstrate super resolution effect.
 * </refsect2>
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "gstnvmaxinevideofx.h"

GST_DEBUG_CATEGORY_STATIC (gst_nv_maxine_videofx_debug_category);
#define GST_CAT_DEFAULT gst_nv_maxine_videofx_debug_category

/* prototypes */


static void gst_nv_maxine_videofx_set_property (GObject * object,
    guint property_id, const GValue * value, GParamSpec * pspec);
static void gst_nv_maxine_videofx_get_property (GObject * object,
    guint property_id, GValue * value, GParamSpec * pspec);
static void gst_nv_maxine_videofx_dispose (GObject * object);
static void gst_nv_maxine_videofx_finalize (GObject * object);

static GstCaps *gst_nv_maxine_videofx_transform_caps (GstBaseTransform * trans,
    GstPadDirection direction, GstCaps * caps, GstCaps * filter);
static gboolean gst_nv_maxine_videofx_set_caps (GstBaseTransform * trans,
    GstCaps * incaps, GstCaps * outcaps);
static gsize gst_nv_maxine_videofx_calculate_output_size(GstNvMaxineVideoFx * maxine, GstCaps * caps);
static gboolean gst_nv_maxine_videofx_transform_size (GstBaseTransform * trans,
    GstPadDirection direction, GstCaps * caps, gsize size, GstCaps * othercaps,
    gsize * othersize);
static gboolean
gst_nv_maxine_videofx_get_unit_size (GstBaseTransform * trans, GstCaps * caps,
                          gsize * size);
static GstFlowReturn gst_nv_maxine_videofx_transform (GstBaseTransform * trans,
    GstBuffer * inbuf, GstBuffer * outbuf);

enum
{
  PROP_0,
  PROP_EFFECT,
  PROP_MODEL_DIR,
  PROP_STRENGTH,
  PROP_MODE,
  PROP_UPSCALE_FACTOR,
  PROP_USE_METADATA,
  PROP_IMAGE_PATH,
};

static GstStaticPadTemplate gst_nv_maxine_videofx_src_template =
        GST_STATIC_PAD_TEMPLATE ("src",
                                 GST_PAD_SRC,
                                 GST_PAD_ALWAYS,
                                 GST_STATIC_CAPS ("video/x-raw, "
                                                  "format = (string) { BGR, I420, NV12 }"));

static GstStaticPadTemplate gst_nv_maxine_videofx_sink_template =
GST_STATIC_PAD_TEMPLATE ("sink",
    GST_PAD_SINK,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS ("video/x-raw, "
                     "format = (string) { BGR, I420, NV12 }"));

G_DEFINE_TYPE_WITH_CODE (GstNvMaxineVideoFx, gst_nv_maxine_videofx, GST_TYPE_BASE_TRANSFORM,
                         GST_DEBUG_CATEGORY_INIT (gst_nv_maxine_videofx_debug_category, "nvmaxinevideofx", 0,
  "debug category for maxine element"));

static void
gst_nv_maxine_videofx_class_init (GstNvMaxineVideoFxClass * klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
    GstBaseTransformClass *base_transform_class = GST_BASE_TRANSFORM_CLASS (klass);

    /* Setting up pads and setting metadata should be moved to
     base_class_init if you intend to subclass this class. */
    gst_element_class_add_static_pad_template (GST_ELEMENT_CLASS(klass),
      &gst_nv_maxine_videofx_src_template);
    gst_element_class_add_static_pad_template (GST_ELEMENT_CLASS(klass),
      &gst_nv_maxine_videofx_sink_template);

    gst_element_class_set_static_metadata (GST_ELEMENT_CLASS(klass),
      "Gstreamer nvidia maxine videoFx plugin",
      "Generic",
      "Plugin to use nvidia maxine sdk in gstreamer pipeline. Only super resolution is supported in this release",
      "voidmainvoid95 <alexpitrolo95@gmail.com>");

    gobject_class->set_property = gst_nv_maxine_videofx_set_property;
    gobject_class->get_property = gst_nv_maxine_videofx_get_property;
    gobject_class->dispose = gst_nv_maxine_videofx_dispose;
    gobject_class->finalize = gst_nv_maxine_videofx_finalize;
    base_transform_class->transform_caps = GST_DEBUG_FUNCPTR (gst_nv_maxine_videofx_transform_caps);
    base_transform_class->set_caps = GST_DEBUG_FUNCPTR (gst_nv_maxine_videofx_set_caps);
    base_transform_class->transform_size = GST_DEBUG_FUNCPTR (gst_nv_maxine_videofx_transform_size);
    base_transform_class->get_unit_size = GST_DEBUG_FUNCPTR (gst_nv_maxine_videofx_get_unit_size);
    base_transform_class->transform = GST_DEBUG_FUNCPTR (gst_nv_maxine_videofx_transform);
    base_transform_class->transform_ip = NULL;

    g_object_class_install_property (gobject_class, PROP_EFFECT,
                                     g_param_spec_string ("effect", "Effect",
                                                          "Effect to use. Must be one of SuperRes, ArtifactReduction, "
                                                          "Upscale, Denoising, GreenScreen, BackgroundBlur, Composition",
                                                          NVVFX_FX_ARTIFACT_REDUCTION,
                                                          G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
    g_object_class_install_property (gobject_class, PROP_MODEL_DIR,
                                     g_param_spec_string ("modeldir", "ModelDir",
                                                          "Directory with Maxine's VideoFx models",
                                                          DEFAULT_MODEL_DIR,
                                                          G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
    g_object_class_install_property (gobject_class, PROP_STRENGTH,
                                     g_param_spec_float ("strength", "Strength",
                                                         "For effect=Upscale, Denoising, BackgroundBlur: "
                                                         "Strength of effect (value between 0 to 1) Default 0.4\n\t\t\t",
                                                         0.0,
                                                         1.0,
                                                         DEFAULT_STRENGTH,
                                                         G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
    g_object_class_install_property (gobject_class, PROP_MODE,
                                     g_param_spec_uint ("mode", "Mode",
                                                         "For effect=ArtifactReduction, SuperRes, GreenScreen: "
                                                         "Effect mode: (0: quality mode, 1 performance mode) Default 1\n\t\t\t",
                                                         0,
                                                         1,
                                                         DEFAULT_MODE,
                                                         G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

    g_object_class_install_property (gobject_class, PROP_UPSCALE_FACTOR,
                                     gst_param_spec_fraction("upscalefactor", "upscaleFactor",
                                                        "For effect=Upscale,SuperRes: "
                                                        "The desired upscale factor (4/3, 3/2, 2, 3, 4) Default 2",
                                                        4, 3, 4, 1, 2, 1,
                                                        G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

    g_object_class_install_property (gobject_class, PROP_USE_METADATA,
                                     g_param_spec_boolean("metadata", "Metadata",
                                                       "For effect=BackgroundBlur,Composition: Get fgMask from buffer metadata",
                                                       FALSE,
                                                       G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
    g_object_class_install_property (gobject_class, PROP_IMAGE_PATH,
                                     g_param_spec_string ("imagepath", "ImagePath",
                                                          "For effect=Composition: Set background image path. Only jpeg supported. "
                                                          "Image must have same resolution as input stream",
                                                          NULL,
                                                          G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
}

static void
gst_nv_maxine_videofx_init (GstNvMaxineVideoFx *maxine)
{

    g_mutex_init (&maxine->mutex);
    g_mutex_lock(&maxine->mutex);
    videofx_init(&maxine->videoFx);
    g_mutex_unlock(&maxine->mutex);
    gst_base_transform_set_qos_enabled(GST_BASE_TRANSFORM (maxine), TRUE);
    gst_base_transform_set_passthrough(GST_BASE_TRANSFORM (maxine), FALSE);
    GST_DEBUG_OBJECT(maxine, "Maxine initialized");
}

void
gst_nv_maxine_videofx_set_property (GObject * object, guint property_id,
    const GValue * value, GParamSpec * pspec)
{
    GstNvMaxineVideoFx *maxine = GST_NV_MAXINE_VIDEOFX(object);

    GST_DEBUG_OBJECT (maxine, "set_property");

    g_mutex_lock (&maxine->mutex);
    switch (property_id) {
        case PROP_EFFECT:
            g_free(maxine->videoFx.effect);
            maxine->videoFx.effect = g_strdup(g_value_get_string(value));
            break;
        case PROP_MODEL_DIR:
            g_free(maxine->videoFx.modelDir);
            maxine->videoFx.modelDir = g_strdup(g_value_get_string(value));
            break;
        case PROP_STRENGTH:
            maxine->videoFx.strength = g_value_get_float(value);
            break;
        case PROP_MODE:
            maxine->videoFx.mode = g_value_get_uint(value);
            break;
        case PROP_UPSCALE_FACTOR:
            maxine->videoFx.upscaleFactor.numerator = gst_value_get_fraction_numerator(value);
            maxine->videoFx.upscaleFactor.denominator = gst_value_get_fraction_denominator(value);
            maxine->videoFx.upscaleFactor.value = (gfloat) maxine->videoFx.upscaleFactor.numerator / (gfloat) maxine->videoFx.upscaleFactor.denominator;
            break;
        case PROP_USE_METADATA:
            maxine->videoFx.useMetadata = g_value_get_boolean(value);
            break;
        case PROP_IMAGE_PATH:
            maxine->videoFx.imagePath = g_strdup(g_value_get_string(value));
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
            break;
    }

    g_mutex_unlock(&maxine->mutex);
}

void
gst_nv_maxine_videofx_get_property (GObject * object, guint property_id,
    GValue * value, GParamSpec * pspec)
{
    GstNvMaxineVideoFx *maxine = GST_NV_MAXINE_VIDEOFX(object);
    GST_DEBUG_OBJECT (maxine, "get_property");
    g_mutex_lock(&maxine->mutex);
    switch (property_id) {
        case PROP_EFFECT:
            g_value_set_string(value, maxine->videoFx.effect);
            break;
        case PROP_MODEL_DIR:
            g_value_set_string(value, maxine->videoFx.modelDir);
            break;
        case PROP_STRENGTH:
            g_value_set_float(value, maxine->videoFx.strength);
            break;
        case PROP_MODE:
            g_value_set_uint(value, maxine->videoFx.mode);
            break;
        case PROP_UPSCALE_FACTOR:
            gst_value_set_fraction(value, maxine->videoFx.upscaleFactor.numerator, maxine->videoFx.upscaleFactor.denominator);
            break;
        case PROP_USE_METADATA:
            g_value_set_boolean(value, maxine->videoFx.useMetadata);
            break;
        case PROP_IMAGE_PATH:
            g_value_set_string(value, maxine->videoFx.imagePath);
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
            break;
    }
   g_mutex_unlock(&maxine->mutex);
}

void
gst_nv_maxine_videofx_dispose (GObject * object)
{
  GstNvMaxineVideoFx *maxine = GST_NV_MAXINE_VIDEOFX(object);

  GST_DEBUG_OBJECT (maxine, "dispose");

  /* clean up as possible.  may be called multiple times */

  G_OBJECT_CLASS (gst_nv_maxine_videofx_parent_class)->dispose (object);
}

void
gst_nv_maxine_videofx_finalize (GObject * object)
{
  GstNvMaxineVideoFx *maxine = GST_NV_MAXINE_VIDEOFX (object);

  GST_DEBUG_OBJECT (maxine, "finalize");

  g_mutex_lock(&maxine->mutex);
  if (maxine->videoFx.initialized == TRUE){
      videofx_cleanup_buffers(&maxine->videoFx);
      videofx_destroy_effect(&maxine->videoFx);
      maxine->videoFx.initialized = FALSE;
  }
  g_free(maxine->videoFx.modelDir);
  g_free(maxine->videoFx.effect);
  if (maxine->videoFx.imagePath){
      g_free(maxine->videoFx.imagePath);
  }
  g_mutex_unlock(&maxine->mutex);
  g_mutex_clear(&maxine->mutex);

  G_OBJECT_CLASS (gst_nv_maxine_videofx_parent_class)->finalize (object);
}

static GstCaps *
gst_nv_maxine_videofx_transform_caps (GstBaseTransform * trans, GstPadDirection direction,
    GstCaps * caps, GstCaps * filter)
{
    GstNvMaxineVideoFx *maxine = GST_NV_MAXINE_VIDEOFX (trans);
    GstCaps *othercaps;

    GST_DEBUG_OBJECT (maxine, "transform_caps");

    othercaps = gst_caps_copy (caps);

    if (!gst_caps_is_empty(caps)){
        GstStructure  *caps_structure = gst_caps_get_structure (caps, 0);
        const GValue *source_width, *source_height, *source_format;
        source_width = gst_structure_get_value(caps_structure, "width");
        source_height = gst_structure_get_value(caps_structure, "height");
        source_format = gst_structure_get_value(caps_structure, "format");

        if (direction == GST_PAD_SINK){
            if (G_VALUE_HOLDS_INT(source_width) && G_VALUE_HOLDS_INT(source_height) && G_VALUE_HOLDS_STRING(source_format)) {
                g_mutex_lock(&maxine->mutex);
                if (maxine->videoFx.buffersNegotiated == FALSE) {
                    videofx_set_buffers_params(&maxine->videoFx,
                                               g_value_get_int(source_width),
                                               g_value_get_int(source_height),
                                               g_value_get_string(source_format));
                    maxine->videoFx.buffersNegotiated = TRUE;
                }
                GValue target_width = { G_TYPE_INT, };
                GValue target_height = { G_TYPE_INT, };
                GstStructure *othercaps_structure;
                othercaps = gst_caps_make_writable(othercaps);
                othercaps_structure = gst_caps_get_structure(othercaps, 0);
                g_value_set_int(&target_width, maxine->videoFx.targetWidth);
                g_value_set_int(&target_height, maxine->videoFx.targetHeight);
                gst_structure_set_value(othercaps_structure, "width", &target_width);
                gst_structure_set_value(othercaps_structure, "height", &target_height);
                g_value_unset(&target_width);
                g_value_unset(&target_height);
                g_mutex_unlock(&maxine->mutex);
            }
        }
    }

    GST_DEBUG_OBJECT (maxine, "direction %d, transformed %" GST_PTR_FORMAT
            " to %" GST_PTR_FORMAT, direction, caps, othercaps);

    if (filter) {
        GstCaps *intersection;

        GST_DEBUG_OBJECT (maxine, "Using filter caps %" GST_PTR_FORMAT, filter);
        intersection = gst_caps_intersect(filter, othercaps);
        gst_caps_unref (othercaps);
        othercaps = intersection;
        GST_DEBUG_OBJECT (maxine, "Intersection %" GST_PTR_FORMAT, othercaps);
    }

    return othercaps;
}

static gboolean
gst_nv_maxine_videofx_set_caps (GstBaseTransform * trans, GstCaps * incaps,
    GstCaps * outcaps)
{
    GstNvMaxineVideoFx *maxine = GST_NV_MAXINE_VIDEOFX (trans);
    NvCV_Status err;

    GST_DEBUG_OBJECT (maxine, "set_caps");
    GST_DEBUG_OBJECT (trans, "INCAPS %" GST_PTR_FORMAT , incaps);
    GST_DEBUG_OBJECT (trans, "OUTCAPS %" GST_PTR_FORMAT , outcaps);

    g_mutex_lock(&maxine->mutex);
    if (maxine->videoFx.initialized == FALSE && maxine->videoFx.buffersNegotiated == TRUE){
        GST_INFO_OBJECT (maxine, "Creating effect");
        CHECK_NvCV_RETURN_CODE(err = videofx_create_effect(&maxine->videoFx));
        GST_INFO_OBJECT (maxine, "Allocating buffers");
        CHECK_NvCV_RETURN_CODE(err = videofx_alloc_buffers(&maxine->videoFx));
        GST_INFO_OBJECT (maxine, "Loading models");
        CHECK_NvCV_RETURN_CODE(err = videofx_load_effect(&maxine->videoFx));
        GST_INFO_OBJECT (maxine, "Model loaded");
        maxine->videoFx.initialized = TRUE;
    }
    g_mutex_unlock(&maxine->mutex);
    return TRUE;

    bail:
    GST_ERROR_OBJECT (maxine, "%s", NvCV_GetErrorStringFromCode(err));
    videofx_cleanup_buffers(&maxine->videoFx);
    videofx_destroy_effect(&maxine->videoFx);
    maxine->videoFx.initialized = FALSE;
    g_mutex_unlock(&maxine->mutex);
    return FALSE;
}

static gsize gst_nv_maxine_videofx_calculate_output_size(GstNvMaxineVideoFx * maxine, GstCaps * caps){
    gsize new_size = 0;
    g_mutex_lock(&maxine->mutex);
    new_size = maxine->videoFx.targetSize;
    g_mutex_unlock(&maxine->mutex);
    return new_size;
}

/* transform size */
static gboolean
gst_nv_maxine_videofx_transform_size (GstBaseTransform * trans, GstPadDirection direction,
    GstCaps * caps, gsize size, GstCaps * othercaps, gsize * othersize)
{
    GstNvMaxineVideoFx *maxine = GST_NV_MAXINE_VIDEOFX (trans);

    GST_DEBUG_OBJECT (maxine, "transform_size");

    // othersize depends only on input caps

    (*othersize) = gst_nv_maxine_videofx_calculate_output_size(maxine, caps);
    return TRUE;
}

static gboolean
gst_nv_maxine_videofx_get_unit_size (GstBaseTransform * trans, GstCaps * caps,
    gsize * size)
{
    GstNvMaxineVideoFx *maxine = GST_NV_MAXINE_VIDEOFX (trans);

    GST_DEBUG_OBJECT (maxine, "get_unit_size");
    // size depends only on input caps

    (*size) = gst_nv_maxine_videofx_calculate_output_size(maxine, caps);
    return TRUE;
}

/* transform */
static GstFlowReturn
gst_nv_maxine_videofx_transform (GstBaseTransform * trans, GstBuffer * inbuf,
    GstBuffer * outbuf)
{
    GstNvMaxineVideoFx *maxine = GST_NV_MAXINE_VIDEOFX (trans);
    GstFlowReturn ret_code;

    GST_DEBUG_OBJECT (maxine, "transform");
    g_mutex_lock(&maxine->mutex);
    if (maxine->videoFx.initialized == FALSE){
        g_mutex_unlock(&maxine->mutex);
        return GST_FLOW_ERROR;
    }
    ret_code = videofx_transform_buffer(&maxine->videoFx, inbuf, outbuf);
    g_mutex_unlock(&maxine->mutex);
    GST_DEBUG_OBJECT (maxine, "transform end");
    return ret_code;
}

static gboolean
plugin_init (GstPlugin * plugin)
{
  return gst_element_register (plugin, "nvmaxinevideofx", GST_RANK_PRIMARY,
                               GST_TYPE_NV_MAXINE_VIDEOFX);
}

/* These are normally defined by the GStreamer build system.
   If you are creating an element to be included in gst-plugins-*,
   remove these, as they're always defined.  Otherwise, edit as
   appropriate for your external plugin package. */
#ifndef VERSION
#define VERSION "0.2.0"
#endif
#ifndef PACKAGE
#define PACKAGE "gst-nvmaxine"
#endif
#ifndef PACKAGE_NAME
#define PACKAGE_NAME "Gstreamer nvmaxine plugin"
#endif
#ifndef GST_PACKAGE_ORIGIN
#define GST_PACKAGE_ORIGIN "https://github.com/voidmainvoid95/gst-nvmaxine.git"
#endif

GST_PLUGIN_DEFINE (GST_VERSION_MAJOR,
    GST_VERSION_MINOR,
    nvmaxinevideofx,
    "Plugin to use nvidia maxine ai platform sdk in gstreamer pipeline. Only videofx is supported.",
    plugin_init, VERSION, "LGPL", PACKAGE_NAME, GST_PACKAGE_ORIGIN)

