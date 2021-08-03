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

#ifndef _GST_NV_MAXINE_VIDEOFX_H_
#define _GST_NV_MAXINE_VIDEOFX_H_

#include <gst/gst.h>
#include <gst/base/gstbasetransform.h>
#include <gst/video/video.h>
#include "videofx.h"

G_BEGIN_DECLS

#define GST_TYPE_NV_MAXINE_VIDEOFX   (gst_nv_maxine_videofx_get_type())
#define GST_NV_MAXINE_VIDEOFX(obj)   (G_TYPE_CHECK_INSTANCE_CAST((obj),GST_TYPE_NV_MAXINE_VIDEOFX,GstNvMaxineVideoFx))
#define GST_NV_MAXINE_VIDEOFX_CLASS(klass)   (G_TYPE_CHECK_CLASS_CAST((klass),GST_TYPE_NV_MAXINE_VIDEOFX,GstNvMaxineVideoFxClass))
#define GST_IS_NV_MAXINE_VIDEOFX(obj)   (G_TYPE_CHECK_INSTANCE_TYPE((obj),GST_TYPE_NV_MAXINE_VIDEOFX))
#define GST_IS_NV_MAXINE_VIDEOFX_CLASS(obj)   (G_TYPE_CHECK_CLASS_TYPE((klass),GST_TYPE_NV_MAXINE_VIDEOFX))

typedef struct _GstNvMaxineVideoFx GstNvMaxineVideoFx;
typedef struct _GstNvMaxineVideoFxClass GstNvMaxineVideoFxClass;

struct _GstNvMaxineVideoFx
{
    GstBaseTransform base_maxine;

    /* Guarding everything below */
    GMutex mutex;
    VideoFx videoFx;
};

struct _GstNvMaxineVideoFxClass
{
  GstBaseTransformClass base_maxine_class;
};

GType gst_nv_maxine_videofx_get_type (void);

G_END_DECLS

#endif
