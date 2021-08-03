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
 * Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 * This software contains source code provided by NVIDIA Corporation.
 */

#ifndef _VIDEOFX_H
#define _VIDEOFX_H

#include <cuda_runtime_api.h>
#include <nvVideoEffects.h>
#include <nvCVImage.h>
#include <gst/gst.h>
#include <math.h>
#include <unistd.h>
#include <stdlib.h>
#include "gstnvmaxinemeta.h"
#include "utils.h"

#define DEFAULT_MODEL_DIR "/usr/local/VideoFX/lib/models"
#define DEFAULT_AR_STRENGTH 1
#define DEFAULT_UPSCALE_FACTOR_NUMERATOR 2
#define DEFAULT_UPSCALE_FACTOR_DENOMINATOR 1
#define NVVFX_FX_COMPOSITION "Composition"

typedef struct _UpscaleFactor{
    gint numerator;
    gint denominator;
    float value;
} UpscaleFactor;


typedef struct _VideoFx {
    CUstream stream;
    gchar *effect, *modelDir, *imagePath;
    gfloat strength;
    UpscaleFactor upscaleFactor;
    gint targetWidth, targetHeight, targetPitch, sourceWidth, sourceHeight, sourcePitch;
    NvCVImage_PixelFormat sourceFormat, targetFormat;
    NvCVImage_ComponentType sourceComponentType, targetComponentType;
    guint sourceLayout, targetLayout;
    gsize sourceSize, targetSize;
    NvVFX_Handle effect_handler, aux_handler;
    NvCVImage srcGpuBuf;
    NvCVImage dstGpuBuf;
    NvCVImage maskGpuBuf;
    NvCVImage bgGpuBuf;
    NvCVImage tmpVFX;
    void *state[1];
    unsigned int state_size;
    gboolean initialized, useMetadata;
} VideoFx;

void videofx_init(VideoFx *videofx);

NvCV_Status videofx_create_effect(VideoFx *videoFx);

static NvCV_Status videofx_create_aux_green_screen(VideoFx *videoFx);

void videofx_destroy_effect(VideoFx *videoFx);

NvCV_Status videofx_alloc_buffers(VideoFx *videoFx);

NvCV_Status videofx_wrap_buffers(VideoFx *videoFx, guint8 *src, NvCVImage *src_wrapper, guint8 *dst, NvCVImage *dst_wrapper);

void videofx_cleanup_buffers(VideoFx *videoFx);

NvCV_Status videofx_load_effect(VideoFx *videoFx);

static NvCV_Status videofx_load_aux_green_screen(VideoFx *videoFx);

void videofx_parse_format(VideoFx *videoFx, GstCaps *in_caps, GstCaps *out_caps);

void videofx_parse_w_h(VideoFx *videoFx, GstCaps *caps);

NvCV_Status videofx_apply_effect(VideoFx *videoFx, guint8 *src, guint8 *dst);

NvCV_Status videofx_apply_blur_with_mask(VideoFx *videoFx, guint8 *src, guint8 *dst, guint8 *mask);

NvCV_Status videofx_apply_blur_with_aux(VideoFx *videoFx, guint8 *src, guint8 *dst);

NvCV_Status videofx_apply_composite_with_mask(VideoFx *videoFx, guint8 *src, guint8 *dst, guint8 *mask);

NvCV_Status videofx_apply_composite_with_aux(VideoFx *videoFx, guint8 *src, guint8 *dst);

GstFlowReturn videofx_transform_buffer(VideoFx *videoFx, GstBuffer *in_buffer, GstBuffer *out_buffer);

#define CHECK_NvCV_RETURN_CODE(err) do { if (0 != (err)){ goto bail; } } while(0)

#endif
