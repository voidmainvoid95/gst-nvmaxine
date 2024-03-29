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

#ifndef _UTILS_H
#define _UTILS_H

#include <cstdio>

#if defined(linux) || defined(unix) || defined(__linux)
	#include <unistd.h>
#elif _WIN32
	#include<io.h>
#endif

#include <jpeglib.h>

int load_image_jpeg(const char *path, unsigned char **dst, int *width, int *height, unsigned int *size);

#endif //_UTILS_H
