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

#include "utils.h"

int load_image_jpeg(const char *path, unsigned char **dst, int *width, int *height, unsigned int *size)
{
    int rc,i;
    struct stat file_info;
    unsigned long jpg_size;
    unsigned char *jpg_buffer;

    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;

    unsigned long bmp_size;
    int row_stride, pixel_size;

    rc = stat(path, &file_info);
    if (rc) {
        return rc;
    }
    jpg_size = file_info.st_size;
    jpg_buffer = (unsigned char*) malloc((size_t)jpg_size + 100);

    int fd = open(path, O_RDONLY);
    i = 0;
    while (i < jpg_size) {
        rc = read(fd, jpg_buffer + i, jpg_size - i);
        i += rc;
    }
    close(fd);

    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);
    jpeg_mem_src(&cinfo, jpg_buffer, jpg_size);

    rc = jpeg_read_header(&cinfo, TRUE);

    if (rc != 1) goto bail;

    jpeg_start_decompress(&cinfo);

    (*width) = cinfo.output_width;
    (*height) = cinfo.output_height;
    pixel_size = cinfo.output_components;

    bmp_size = (*width) * (*height) * pixel_size;
    (*dst) = (unsigned char *) malloc(bmp_size);
    (*size) = bmp_size;

    row_stride = (*width) * pixel_size;

    while (cinfo.output_scanline < cinfo.output_height) {
        unsigned char *buffer_array[1];
        buffer_array[0] = (*dst) + (cinfo.output_scanline) * row_stride;
        jpeg_read_scanlines(&cinfo, buffer_array, 1);
    }
    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);
    bail:
    free(jpg_buffer);
    return rc;
}