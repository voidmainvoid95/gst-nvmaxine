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

#include "gstnvmaxinemeta.h"

GType
gst_nv_maxine_meta_api_get_type (void)
{
    static volatile GType type;
    static const gchar *tags[] = { NULL };

    if (g_once_init_enter (&type)) {
        GType _type = gst_meta_api_type_register ("GstNvMaxineMetaAPI", tags);
        g_once_init_leave (&type, _type);
    }
    return type;
}

gboolean
gst_nv_maxine_meta_init(GstMeta *meta, gpointer params, GstBuffer *buffer)
{
    GstNvMaxineMeta* maxine_meta = (GstNvMaxineMeta *)meta;

    maxine_meta->buffer = NULL;

    return TRUE;
}

gboolean
gst_nv_maxine_meta_transform (GstBuffer *dest_buf,
                            GstMeta *src_meta,
                            GstBuffer *src_buf,
                            GQuark type,
                            gpointer data) {

    GstNvMaxineMeta* src_maxine_meta = (GstNvMaxineMeta*)src_meta;
    GstNvMaxineMeta* dest_maxine_meta = gst_nv_maxine_meta_add(dest_buf);
    dest_maxine_meta->buffer = gst_buffer_copy(src_maxine_meta->buffer);

    return TRUE;
}

void
gst_nv_maxine_meta_free (GstMeta *meta, GstBuffer *buffer) {
    GstNvMaxineMeta* maxine_meta = (GstNvMaxineMeta *)meta;
    if(maxine_meta->buffer) gst_buffer_unref(maxine_meta->buffer);
}

const GstMetaInfo *
gst_nv_maxine_meta_get_info (void)
{
    static const GstMetaInfo *meta_info = NULL;

    if (g_once_init_enter (&meta_info)) {
        const GstMetaInfo *meta =
                gst_meta_register (gst_nv_maxine_meta_api_get_type(), "GstNvMaxineMeta",
                                   sizeof (GstNvMaxineMeta), (GstMetaInitFunction)gst_nv_maxine_meta_init,
                                   (GstMetaFreeFunction)gst_nv_maxine_meta_free, (GstMetaTransformFunction) gst_nv_maxine_meta_transform);
        g_once_init_leave (&meta_info, meta);
    }
    return meta_info;
}
