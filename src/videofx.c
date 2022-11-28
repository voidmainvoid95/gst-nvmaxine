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

#include "videofx.h"
#include <stdlib.h>

void videofx_init(VideoFx *videofx)
{
    videofx->effect = g_strdup(NVVFX_FX_ARTIFACT_REDUCTION);
    videofx->modelDir = g_strdup(DEFAULT_MODEL_DIR);
    videofx->strength = DEFAULT_AR_STRENGTH;
    videofx->upscaleFactor.numerator = DEFAULT_UPSCALE_FACTOR_NUMERATOR;
    videofx->upscaleFactor.denominator = DEFAULT_UPSCALE_FACTOR_DENOMINATOR;
    videofx->upscaleFactor.value = (gfloat) videofx->upscaleFactor.numerator / (gfloat) videofx->upscaleFactor.denominator;
    videofx->state[0] = NULL;
    videofx->useMetadata = FALSE;
    videofx->initialized = FALSE;
    videofx->buffersNegotiated = FALSE;
    videofx->imagePath = NULL;
    videofx->effect_handler = NULL;
    videofx->aux_handler = NULL;
}
static NvCV_Status videofx_create_aux_green_screen(VideoFx *videoFx)
{
    NvCV_Status vfxErr;
    CHECK_NvCV_RETURN_CODE(vfxErr = NvVFX_CreateEffect(NVVFX_FX_GREEN_SCREEN, &videoFx->aux_handler));
    CHECK_NvCV_RETURN_CODE(vfxErr = NvVFX_SetCudaStream(videoFx->aux_handler, NVVFX_CUDA_STREAM, videoFx->stream));
    CHECK_NvCV_RETURN_CODE(vfxErr = NvVFX_SetString(videoFx->aux_handler, NVVFX_MODEL_DIRECTORY, videoFx->modelDir));
    bail:
    return vfxErr;
}


NvCV_Status videofx_create_effect(VideoFx *videoFx)
{
    NvCV_Status vfxErr = NVCV_SUCCESS;
    cudaStreamCreate(&videoFx->stream);
    if (!g_strcmp0(videoFx->effect, NVVFX_FX_COMPOSITION)){
        if (!videoFx->useMetadata) {
            CHECK_NvCV_RETURN_CODE(vfxErr = videofx_create_aux_green_screen(videoFx));
        }
    }else{
        CHECK_NvCV_RETURN_CODE(vfxErr = NvVFX_CreateEffect(videoFx->effect, &videoFx->effect_handler));
        CHECK_NvCV_RETURN_CODE(vfxErr = NvVFX_SetCudaStream(videoFx->effect_handler, NVVFX_CUDA_STREAM, videoFx->stream));
        if (!g_strcmp0(videoFx->effect, NVVFX_FX_BGBLUR)){
            if (!videoFx->useMetadata){
                CHECK_NvCV_RETURN_CODE(vfxErr = videofx_create_aux_green_screen(videoFx));
            }
        }
        else{
            CHECK_NvCV_RETURN_CODE(vfxErr = NvVFX_SetString(videoFx->effect_handler, NVVFX_MODEL_DIRECTORY, videoFx->modelDir));
        }
    }
    bail:
    return vfxErr;
}

void videofx_destroy_effect(VideoFx *videoFx)
{
    if ((!g_strcmp0(videoFx->effect, NVVFX_FX_BGBLUR) || !g_strcmp0(videoFx->effect, NVVFX_FX_COMPOSITION)) && !videoFx->useMetadata){
        NvVFX_DestroyEffect(videoFx->aux_handler);
        videoFx->aux_handler = NULL;
    }
    if(videoFx->effect_handler){
        NvVFX_DestroyEffect(videoFx->effect_handler);
        videoFx->effect_handler = NULL;
    }
    cudaStreamDestroy(videoFx->stream);
}

NvCV_Status videofx_alloc_buffers(VideoFx *videoFx)
{
    NvCV_Status vfxErr;
    if (!g_strcmp0(videoFx->effect, NVVFX_FX_ARTIFACT_REDUCTION) ||
        !g_strcmp0(videoFx->effect, NVVFX_FX_SUPER_RES) ||
        !g_strcmp0(videoFx->effect, NVVFX_FX_DENOISING)) {
        CHECK_NvCV_RETURN_CODE(vfxErr = NvCVImage_Alloc(&videoFx->srcGpuBuf,
                                                        videoFx->sourceWidth,
                                                        videoFx->sourceHeight,
                                                        NVCV_BGR,
                                                        NVCV_F32,
                                                        NVCV_PLANAR,
                                                        NVCV_GPU,
                                                        1
                                                        ));
        CHECK_NvCV_RETURN_CODE(vfxErr = NvCVImage_Alloc(&videoFx->dstGpuBuf,
                                                        videoFx->targetWidth,
                                                        videoFx->targetHeight,
                                                        NVCV_BGR,
                                                        NVCV_F32,
                                                        NVCV_PLANAR,
                                                        NVCV_GPU,
                                                        1
                                                        ));
    }
    else if (!g_strcmp0(videoFx->effect, NVVFX_FX_SR_UPSCALE)) {
        CHECK_NvCV_RETURN_CODE(vfxErr = NvCVImage_Alloc(&videoFx->srcGpuBuf,
                                                        videoFx->sourceWidth,
                                                        videoFx->sourceHeight,
                                                        NVCV_RGBA,
                                                        NVCV_U8,
                                                        NVCV_INTERLEAVED,
                                                        NVCV_GPU,
                                                        32
                                             ));
        CHECK_NvCV_RETURN_CODE(vfxErr = NvCVImage_Alloc(&videoFx->dstGpuBuf,
                                                        videoFx->targetWidth,
                                                        videoFx->targetHeight,
                                                        NVCV_RGBA,
                                                        NVCV_U8,
                                                        NVCV_INTERLEAVED,
                                                        NVCV_GPU,
                                                        32
                                             ));
    } else if (!g_strcmp0(videoFx->effect, NVVFX_FX_GREEN_SCREEN)) {
        CHECK_NvCV_RETURN_CODE(vfxErr = NvCVImage_Alloc(&videoFx->srcGpuBuf,
                                                        videoFx->sourceWidth,
                                                        videoFx->sourceHeight,
                                                        NVCV_BGR,
                                                        NVCV_U8,
                                                        NVCV_CHUNKY,
                                                        NVCV_GPU,
                                                        32
        ));
        CHECK_NvCV_RETURN_CODE(vfxErr = NvCVImage_Alloc(&videoFx->dstGpuBuf,
                                                        videoFx->targetWidth,
                                                        videoFx->targetHeight,
                                                        NVCV_A,
                                                        NVCV_U8,
                                                        NVCV_CHUNKY,
                                                        NVCV_GPU,
                                                        32
        ));
    } else if (!g_strcmp0(videoFx->effect, NVVFX_FX_BGBLUR) || !g_strcmp0(videoFx->effect, NVVFX_FX_COMPOSITION)) {
        CHECK_NvCV_RETURN_CODE(vfxErr = NvCVImage_Alloc(&videoFx->srcGpuBuf,
                                                        videoFx->sourceWidth,
                                                        videoFx->sourceHeight,
                                                        NVCV_BGR,
                                                        NVCV_U8,
                                                        NVCV_CHUNKY,
                                                        NVCV_GPU,
                                                        32
        ));
        CHECK_NvCV_RETURN_CODE(vfxErr = NvCVImage_Alloc(&videoFx->maskGpuBuf,
                                                        videoFx->sourceWidth,
                                                        videoFx->sourceHeight,
                                                        NVCV_A,
                                                        NVCV_U8,
                                                        NVCV_CHUNKY,
                                                        NVCV_GPU,
                                                        32
        ));
        CHECK_NvCV_RETURN_CODE(vfxErr = NvCVImage_Alloc(&videoFx->dstGpuBuf,
                                                        videoFx->targetWidth,
                                                        videoFx->targetHeight,
                                                        NVCV_BGR,
                                                        NVCV_U8,
                                                        NVCV_CHUNKY,
                                                        NVCV_GPU,
                                                        32
        ));
        if (!g_strcmp0(videoFx->effect, NVVFX_FX_COMPOSITION)){
            // FIXME libjpeg limitation
            CHECK_NvCV_RETURN_CODE(vfxErr = NvCVImage_Alloc(&videoFx->bgGpuBuf,
                                                            videoFx->sourceWidth,
                                                            videoFx->sourceHeight,
                                                            NVCV_RGB,
                                                            NVCV_U8,
                                                            NVCV_CHUNKY,
                                                            NVCV_GPU,
                                                            32
            ));
            if (videoFx->imagePath){
                guint8 *image;
                gint width, height;
                guint size;
                int rc = load_image_jpeg(videoFx->imagePath, &image, &width, &height, &size);
                if (rc != 1){
                    return NVCV_ERR_BUFFER;
                }
                cudaMemcpyAsync(videoFx->bgGpuBuf.pixels, image, size, cudaMemcpyHostToDevice, videoFx->stream);
                cudaStreamSynchronize(videoFx->stream);
                free(image);
            }
            else cudaMemsetAsync(videoFx->bgGpuBuf.pixels, 0, videoFx->sourceSize, videoFx->stream);
        }
    } else return NVCV_ERR_PARAMETER;

    CHECK_NvCV_RETURN_CODE(vfxErr = NvCVImage_Alloc(&videoFx->tmpVFX,
                                                    videoFx->sourceWidth,
                                                    videoFx->sourceHeight,
                                                    videoFx->sourceFormat,
                                                    videoFx->sourceComponentType,
                                                    videoFx->sourceLayout,
                                                    NVCV_GPU,
                                                    0
    ));
    CHECK_NvCV_RETURN_CODE(vfxErr = NvCVImage_Realloc(&videoFx->tmpVFX,
                                                      videoFx->targetWidth,
                                                      videoFx->targetHeight,
                                                      videoFx->targetFormat,
                                                      videoFx->targetComponentType,
                                                      videoFx->targetLayout,
                                                      NVCV_GPU,
                                                      0
    ));

    bail:
    return vfxErr;
}

NvCV_Status videofx_wrap_buffers(VideoFx *videoFx, guint8 *src, NvCVImage *src_wrapper, guint8 *dst, NvCVImage *dst_wrapper){
    NvCV_Status vfxErr;
    CHECK_NvCV_RETURN_CODE(vfxErr = NvCVImage_Init(src_wrapper,
                                                   videoFx->sourceWidth,
                                                   videoFx->sourceHeight,
                                                   videoFx->sourcePitch,
                                                   src,
                                                   videoFx->sourceFormat,
                                                   videoFx->sourceComponentType,
                                                   videoFx->sourceLayout,
                                                   NVCV_CPU
    ));
    CHECK_NvCV_RETURN_CODE(vfxErr = NvCVImage_Init(dst_wrapper,
                                                   videoFx->targetWidth,
                                                   videoFx->targetHeight,
                                                   videoFx->targetPitch,
                                                   dst,
                                                   videoFx->targetFormat,
                                                   videoFx->targetComponentType,
                                                   videoFx->targetLayout,
                                                   NVCV_CPU
    ));
    bail:
    return vfxErr;

}

void videofx_cleanup_buffers(VideoFx *videoFx){
    NvCVImage_Dealloc(&videoFx->srcGpuBuf);
    NvCVImage_Dealloc(&videoFx->dstGpuBuf);
    NvCVImage_Dealloc(&videoFx->tmpVFX);
    if (!g_strcmp0(videoFx->effect, NVVFX_FX_BGBLUR) || !g_strcmp0(videoFx->effect, NVVFX_FX_COMPOSITION)) {
        NvCVImage_Dealloc(&videoFx->maskGpuBuf);
        if (!g_strcmp0(videoFx->effect, NVVFX_FX_COMPOSITION)){
            NvCVImage_Dealloc(&videoFx->bgGpuBuf);
        }
    }
    if (videoFx->state[0]){
        cudaFree(videoFx->state[0]);
    }
}

static NvCV_Status videofx_load_aux_green_screen(VideoFx *videoFx){
    NvCV_Status vfxErr;
    CHECK_NvCV_RETURN_CODE(vfxErr = NvVFX_SetImage(videoFx->aux_handler, NVVFX_INPUT_IMAGE, &videoFx->srcGpuBuf));
    CHECK_NvCV_RETURN_CODE(vfxErr = NvVFX_SetImage(videoFx->aux_handler, NVVFX_OUTPUT_IMAGE, &videoFx->maskGpuBuf));
    CHECK_NvCV_RETURN_CODE(vfxErr = NvVFX_SetU32(videoFx->aux_handler, NVVFX_MODE, (unsigned int)roundf(1 - videoFx->strength)));
    CHECK_NvCV_RETURN_CODE(vfxErr = NvVFX_Load(videoFx->aux_handler));
    bail:
    return vfxErr;
}

NvCV_Status videofx_load_effect(VideoFx *videoFx){
    NvCV_Status vfxErr= NVCV_SUCCESS;
    if (!g_strcmp0(videoFx->effect, NVVFX_FX_BGBLUR)){
        CHECK_NvCV_RETURN_CODE(vfxErr = NvVFX_SetImage(videoFx->effect_handler, NVVFX_INPUT_IMAGE_0, &videoFx->srcGpuBuf));
        CHECK_NvCV_RETURN_CODE(vfxErr = NvVFX_SetImage(videoFx->effect_handler, NVVFX_INPUT_IMAGE_1, &videoFx->maskGpuBuf));
        CHECK_NvCV_RETURN_CODE(vfxErr = NvVFX_SetImage(videoFx->effect_handler, NVVFX_OUTPUT_IMAGE, &videoFx->dstGpuBuf));
        if (!videoFx->useMetadata){
            CHECK_NvCV_RETURN_CODE(vfxErr = videofx_load_aux_green_screen(videoFx));
        }
    } else if (!g_strcmp0(videoFx->effect, NVVFX_FX_COMPOSITION)){
        if (!videoFx->useMetadata){
            CHECK_NvCV_RETURN_CODE(vfxErr = videofx_load_aux_green_screen(videoFx));
        }
        return vfxErr;
    }
    else{
        CHECK_NvCV_RETURN_CODE(vfxErr = NvVFX_SetImage(videoFx->effect_handler, NVVFX_INPUT_IMAGE, &videoFx->srcGpuBuf));
        CHECK_NvCV_RETURN_CODE(vfxErr = NvVFX_SetImage(videoFx->effect_handler, NVVFX_OUTPUT_IMAGE, &videoFx->dstGpuBuf));
    }


    if (!g_strcmp0(videoFx->effect, NVVFX_FX_ARTIFACT_REDUCTION) || !g_strcmp0(videoFx->effect, NVVFX_FX_SUPER_RES)){
        CHECK_NvCV_RETURN_CODE(vfxErr = NvVFX_SetU32(videoFx->effect_handler, NVVFX_STRENGTH, (unsigned int)roundf(videoFx->strength)));
    } else if (!g_strcmp0(videoFx->effect, NVVFX_FX_SR_UPSCALE) ||
                !g_strcmp0(videoFx->effect, NVVFX_FX_DENOISING) ||
                !g_strcmp0(videoFx->effect, NVVFX_FX_BGBLUR)){
            CHECK_NvCV_RETURN_CODE(vfxErr = NvVFX_SetF32(videoFx->effect_handler, NVVFX_STRENGTH, videoFx->strength));
    } else if(!g_strcmp0(videoFx->effect, NVVFX_FX_GREEN_SCREEN)){
        CHECK_NvCV_RETURN_CODE(vfxErr = NvVFX_SetU32(videoFx->effect_handler, NVVFX_MODE, (unsigned int)roundf(videoFx->strength)));
    }

    if (!g_strcmp0(videoFx->effect, NVVFX_FX_DENOISING)){
        CHECK_NvCV_RETURN_CODE(vfxErr = NvVFX_GetU32(videoFx->effect_handler, NVVFX_STATE_SIZE, &videoFx->state_size));
        cudaMalloc(&videoFx->state[0], videoFx->state_size);
        cudaMemsetAsync(videoFx->state[0], 0, videoFx->state_size, videoFx->stream);
        CHECK_NvCV_RETURN_CODE(vfxErr = NvVFX_SetObject(videoFx->effect_handler, NVVFX_STATE, (void*)videoFx->state));
    }
    CHECK_NvCV_RETURN_CODE(vfxErr = NvVFX_Load(videoFx->effect_handler));

bail:
    return vfxErr;
}

void videofx_set_buffers_params(VideoFx *videoFx, gint width, gint height, const gchar *format){
    videoFx->sourceWidth = width;
    videoFx->sourceHeight = height;
    if (!g_strcmp0(videoFx->effect, NVVFX_FX_SUPER_RES) || !g_strcmp0(videoFx->effect, NVVFX_FX_SR_UPSCALE)){
        videoFx->targetHeight = (guint) ceilf(videoFx->sourceHeight * videoFx->upscaleFactor.value);
        videoFx->targetWidth = videoFx->sourceWidth * videoFx->targetHeight / videoFx->sourceHeight;
    }
    else{
        videoFx->targetWidth = videoFx->sourceWidth;
        videoFx->targetHeight = videoFx->sourceHeight;
    }

    if (!g_strcmp0(format, "BGR")){
        videoFx->sourceFormat = NVCV_BGR;
        videoFx->sourceComponentType = NVCV_U8;
        videoFx->sourceLayout = NVCV_INTERLEAVED;
        videoFx->sourceSize = (gsize) videoFx->sourceWidth * videoFx->sourceHeight * 3 * sizeof(guint8);
        videoFx->sourcePitch = videoFx->sourceWidth * 3;

        videoFx->targetFormat = NVCV_BGR;
        videoFx->targetComponentType = NVCV_U8;
        videoFx->targetLayout = NVCV_INTERLEAVED;
        videoFx->targetSize = (gsize) videoFx->targetWidth * videoFx->targetHeight * 3 * sizeof(guint8);
        videoFx->targetPitch = videoFx->targetWidth * 3;
    }
    else if (!g_strcmp0(format, "I420")){
        videoFx->sourceFormat = NVCV_YUV420;
        videoFx->sourceComponentType = NVCV_U8;
        videoFx->sourceLayout = NVCV_YUV;
        videoFx->sourceSize = (gsize) videoFx->sourceWidth * videoFx->sourceHeight * 3 / 2 * sizeof(guint8);
        videoFx->sourcePitch = videoFx->sourceWidth;

        videoFx->targetFormat = NVCV_YUV420;
        videoFx->targetComponentType = NVCV_U8;
        videoFx->targetLayout = NVCV_YUV;
        videoFx->targetSize = (gsize) videoFx->targetWidth * videoFx->targetHeight * 3 / 2 * sizeof(guint8);
        videoFx->targetPitch = videoFx->targetWidth;
    }
    else if (!g_strcmp0(format, "NV12")){
        videoFx->sourceFormat = NVCV_YUV420;
        videoFx->sourceComponentType = NVCV_U8;
        videoFx->sourceLayout = NVCV_NV12;
        videoFx->sourceSize = (gsize) videoFx->sourceWidth * videoFx->sourceHeight * 3 / 2 * sizeof(guint8);
        videoFx->sourcePitch = videoFx->sourceWidth;

        videoFx->targetFormat = NVCV_YUV420;
        videoFx->targetComponentType = NVCV_U8;
        videoFx->targetLayout = NVCV_NV12;
        videoFx->targetSize = (gsize) videoFx->targetWidth * videoFx->targetHeight * 3 / 2 * sizeof(guint8);
        videoFx->targetPitch = videoFx->targetWidth;
    }

    if (!g_strcmp0(videoFx->effect, NVVFX_FX_GREEN_SCREEN)){
        videoFx->targetFormat = NVCV_A;
        videoFx->targetComponentType = NVCV_U8;
        videoFx->targetLayout = NVCV_CHUNKY;
    }
}

NvCV_Status videofx_apply_effect(VideoFx *videoFx, guint8 *src, guint8 *dst)
{
    NvCV_Status vfxErr;
    NvCVImage srcWrapper, dstWrapper;
    CHECK_NvCV_RETURN_CODE(vfxErr = videofx_wrap_buffers(videoFx, src, &srcWrapper, dst, &dstWrapper));
    CHECK_NvCV_RETURN_CODE(vfxErr = NvCVImage_Transfer(&srcWrapper, &videoFx->srcGpuBuf, 1.f / 255.f, videoFx->stream, &videoFx->tmpVFX));
    CHECK_NvCV_RETURN_CODE(vfxErr = NvVFX_Run(videoFx->effect_handler, 0));
    CHECK_NvCV_RETURN_CODE(vfxErr = NvCVImage_Transfer(&videoFx->dstGpuBuf, &dstWrapper, 255.f, videoFx->stream, &videoFx->tmpVFX));
    if (!g_strcmp0(videoFx->effect, NVVFX_FX_DENOISING)){
        cudaMemsetAsync(videoFx->state[0], 0, videoFx->state_size, videoFx->stream);
    }

    bail:
    return vfxErr;
}

NvCV_Status videofx_apply_blur_with_mask(VideoFx *videoFx, guint8 *src, guint8 *dst, guint8 *mask)
{
    NvCV_Status vfxErr;
    NvCVImage srcWrapper, dstWrapper, maskWrapper;
    CHECK_NvCV_RETURN_CODE(vfxErr = videofx_wrap_buffers(videoFx, src, &srcWrapper, dst, &dstWrapper));
    CHECK_NvCV_RETURN_CODE(vfxErr = NvCVImage_Init(&maskWrapper,
                                                   videoFx->sourceWidth,
                                                   videoFx->sourceHeight,
                                                   videoFx->sourcePitch,
                                                   mask,
                                                   NVCV_A,
                                                   NVCV_U8,
                                                   NVCV_CHUNKY,
                                                   NVCV_CPU
    ));
    CHECK_NvCV_RETURN_CODE(vfxErr = NvCVImage_Transfer(&srcWrapper, &videoFx->srcGpuBuf, 1.f / 255.f, videoFx->stream, &videoFx->tmpVFX));
    CHECK_NvCV_RETURN_CODE(vfxErr = NvCVImage_Transfer(&maskWrapper, &videoFx->maskGpuBuf, 1.f / 255.f, videoFx->stream, NULL));
    CHECK_NvCV_RETURN_CODE(vfxErr = NvVFX_Run(videoFx->effect_handler, 0));
    CHECK_NvCV_RETURN_CODE(vfxErr = NvCVImage_Transfer(&videoFx->dstGpuBuf, &dstWrapper, 255.f, videoFx->stream, &videoFx->tmpVFX));
    bail:
    return vfxErr;
}

NvCV_Status videofx_apply_blur_with_aux(VideoFx *videoFx, guint8 *src, guint8 *dst)
{
    NvCV_Status vfxErr;
    NvCVImage srcWrapper, dstWrapper;
    CHECK_NvCV_RETURN_CODE(vfxErr = videofx_wrap_buffers(videoFx, src, &srcWrapper, dst, &dstWrapper));
    CHECK_NvCV_RETURN_CODE(vfxErr = NvCVImage_Transfer(&srcWrapper, &videoFx->srcGpuBuf, 1.f / 255.f, videoFx->stream, &videoFx->tmpVFX));
    CHECK_NvCV_RETURN_CODE(vfxErr = NvVFX_Run(videoFx->aux_handler, 0));
    CHECK_NvCV_RETURN_CODE(vfxErr = NvVFX_Run(videoFx->effect_handler, 0));
    CHECK_NvCV_RETURN_CODE(vfxErr = NvCVImage_Transfer(&videoFx->dstGpuBuf, &dstWrapper, 255.f, videoFx->stream, &videoFx->tmpVFX));
    bail:
    return vfxErr;
}

NvCV_Status videofx_apply_composite_with_mask(VideoFx *videoFx, guint8 *src, guint8 *dst, guint8 *mask)
{
    NvCV_Status vfxErr;
    NvCVImage srcWrapper, dstWrapper, maskWrapper;
    CHECK_NvCV_RETURN_CODE(vfxErr = videofx_wrap_buffers(videoFx, src, &srcWrapper, dst, &dstWrapper));
    CHECK_NvCV_RETURN_CODE(vfxErr = NvCVImage_Init(&maskWrapper,
                                                   videoFx->sourceWidth,
                                                   videoFx->sourceHeight,
                                                   videoFx->sourcePitch,
                                                   mask,
                                                   NVCV_A,
                                                   NVCV_U8,
                                                   NVCV_CHUNKY,
                                                   NVCV_CPU
    ));
    CHECK_NvCV_RETURN_CODE(vfxErr = NvCVImage_Transfer(&srcWrapper, &videoFx->srcGpuBuf, 1.f / 255.f, videoFx->stream, &videoFx->tmpVFX));
    CHECK_NvCV_RETURN_CODE(vfxErr = NvCVImage_Transfer(&maskWrapper, &videoFx->maskGpuBuf, 1.f / 255.f, videoFx->stream, NULL));
    CHECK_NvCV_RETURN_CODE(vfxErr = NvCVImage_Composite(&videoFx->srcGpuBuf, &videoFx->bgGpuBuf, &videoFx->maskGpuBuf, &videoFx->dstGpuBuf, videoFx->stream));
    CHECK_NvCV_RETURN_CODE(vfxErr = NvCVImage_Transfer(&videoFx->dstGpuBuf, &dstWrapper, 255.f, videoFx->stream, &videoFx->tmpVFX));
    bail:
    return vfxErr;
}

NvCV_Status videofx_apply_composite_with_aux(VideoFx *videoFx, guint8 *src, guint8 *dst)
{
    NvCV_Status vfxErr;
    NvCVImage srcWrapper, dstWrapper;
    CHECK_NvCV_RETURN_CODE(vfxErr = videofx_wrap_buffers(videoFx, src, &srcWrapper, dst, &dstWrapper));
    CHECK_NvCV_RETURN_CODE(vfxErr = NvCVImage_Transfer(&srcWrapper, &videoFx->srcGpuBuf, 1.f / 255.f, videoFx->stream, &videoFx->tmpVFX));
    CHECK_NvCV_RETURN_CODE(vfxErr = NvVFX_Run(videoFx->aux_handler, 0));
    CHECK_NvCV_RETURN_CODE(vfxErr = NvCVImage_Composite(&videoFx->srcGpuBuf, &videoFx->bgGpuBuf, &videoFx->maskGpuBuf, &videoFx->dstGpuBuf, videoFx->stream));
    CHECK_NvCV_RETURN_CODE(vfxErr = NvCVImage_Transfer(&videoFx->dstGpuBuf, &dstWrapper, 255.f, videoFx->stream, &videoFx->tmpVFX));
    bail:
    return vfxErr;
}

GstFlowReturn videofx_transform_buffer(VideoFx *videoFx, GstBuffer *in_buffer, GstBuffer *out_buffer)
{
    GstMapInfo inbuf_map, outbuf_map;
    GstNvMaxineMeta *meta = NULL;
    NvCV_Status err;

    if (!gst_buffer_map(in_buffer, &inbuf_map, GST_MAP_READ)) {
        return GST_FLOW_ERROR;
    }

    if (!inbuf_map.data){
        gst_buffer_unmap(out_buffer, &inbuf_map);
        return GST_FLOW_ERROR;
    }

    if (!gst_buffer_map(out_buffer, &outbuf_map, GST_MAP_WRITE)) {
        gst_buffer_unmap(in_buffer, &inbuf_map);
        return GST_FLOW_ERROR;
    }

    if (!g_strcmp0(videoFx->effect, NVVFX_FX_BGBLUR) || !g_strcmp0(videoFx->effect, NVVFX_FX_COMPOSITION)){
        if (videoFx->useMetadata){
            GstMapInfo mask_map;
            meta = gst_nv_maxine_meta_get(in_buffer);
            if (meta->buffer){
                if (!gst_buffer_map(meta->buffer, &mask_map, GST_MAP_READ)) {
                    gst_buffer_unmap(in_buffer, &inbuf_map);
                    gst_buffer_unmap(out_buffer, &inbuf_map);
                    return GST_FLOW_ERROR;
                }
                if (!g_strcmp0(videoFx->effect, NVVFX_FX_BGBLUR) ) {
                    err = videofx_apply_blur_with_mask(videoFx, inbuf_map.data, outbuf_map.data, mask_map.data);
                }
                else{
                    err = videofx_apply_composite_with_mask(videoFx, inbuf_map.data, outbuf_map.data, mask_map.data);
                }
                gst_buffer_unmap(meta->buffer, &mask_map);
                gst_buffer_unmap(out_buffer, &outbuf_map);
            }
            else{
                return GST_FLOW_ERROR;
            }
        } else{
            if (!g_strcmp0(videoFx->effect, NVVFX_FX_BGBLUR) ){
                err = videofx_apply_blur_with_aux(videoFx, inbuf_map.data, outbuf_map.data);
            }
            else{
                err = videofx_apply_composite_with_aux(videoFx, inbuf_map.data, outbuf_map.data);
            }
            gst_buffer_unmap(out_buffer, &outbuf_map);
        }
    }
    else{
        if (!g_strcmp0(videoFx->effect, NVVFX_FX_GREEN_SCREEN)){
            memcpy(outbuf_map.data, inbuf_map.data, videoFx->sourceSize);
            gst_buffer_unmap(out_buffer, &outbuf_map);
            meta = gst_nv_maxine_meta_add(out_buffer);
            gsize fg_mask_size =  (gsize) videoFx->sourceWidth * videoFx->sourceHeight * sizeof(guint8);
            meta->buffer = gst_buffer_new_allocate(NULL, fg_mask_size, NULL);
            if (!gst_buffer_map(meta->buffer, &outbuf_map, GST_MAP_WRITE)) {
                gst_buffer_unmap(in_buffer, &inbuf_map);
                return GST_FLOW_ERROR;
            }
        }
        err = videofx_apply_effect(videoFx, inbuf_map.data, outbuf_map.data);
        if (meta) gst_buffer_unmap(meta->buffer, &outbuf_map);
        else gst_buffer_unmap(out_buffer, &outbuf_map);
    }
    gst_buffer_unmap(in_buffer, &inbuf_map);
    if (err == NVCV_SUCCESS){
        return GST_FLOW_OK;
    }
    else{
        return GST_FLOW_ERROR;
    }
}