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

#ifndef _GST_NV_MAXINE_META_H
#define _GST_NV_MAXINE_META_H

#include <gst/gst.h>
#include <gst/gstmeta.h>

#if _WIN32
#define EXPORT_API __declspec(dllexport)
#else
#define EXPORT_API
#endif

G_BEGIN_DECLS

typedef struct _GstNvMaxineMeta GstNvMaxineMeta;

struct _GstNvMaxineMeta {
    GstMeta meta;
    GstBuffer *buffer;
};

EXPORT_API GType gst_nv_maxine_meta_api_get_type (void);
EXPORT_API const GstMetaInfo* gst_nv_maxine_meta_get_info (void);
EXPORT_API GstNvMaxineMeta* gst_nv_maxine_meta_get(GstBuffer *buf);
EXPORT_API GstNvMaxineMeta* gst_nv_maxine_meta_add(GstBuffer *buf);
G_END_DECLS

#endif
