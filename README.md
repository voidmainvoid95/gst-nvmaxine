# GST-NVMAXINE
Gstreamer plugin that allows use of NVIDIA Maxine<sup>TM</sup> sdk in a generic pipeline.

This plugin is intended for use with NVIDIA hardware.

Visit https://developer.nvidia.com/maxine-getting-started for a list of
supported GPU's.

At this moment only videofx sdk is supported.

## Supported videofx effects
* ArtifactReduction
* Upscale
* SuperRes
* Denoising
* GreenScreen
* BackgroundBlur

Each filter have a limitation regarding the max and min supported resolution.
Read **NVIDIA_Video_Effects_SDK_Programming_Guide_Linux.pdf** for more information.

## Extra videofx effects
* Composition

This effect allows you to change the background using foreground mask provided by GreenScreen with a single jpeg.

## Build
Retrieve **NVIDIA_VFX_SDK_Ubuntu18.04_0.6.5.0.tgz** from  https://developer.nvidia.com/maxine-getting-started.
   **README_quickstart.md** present inside archive provides download links for the requested dependencies.

```
     cudnn-11.3-linux-x64-v8.2.1.32.tgz 
     TensorRT-8.0.1.6.Linux.x86_64-gnu.cuda-11.3.cudnn8.2.tar.gz
```   

Meson is currently unsupported use cmake. 

```shell
mkdir build && cd build
cmake .. && make -j$(nproc)
make install
```

# Using Dockerfile
This repo provide a Dockerfile for development and testing. To build image you must
provide archives downloaded before:
```
     cudnn-11.3-linux-x64-v8.2.1.32.tgz 
     TensorRT-8.0.1.6.Linux.x86_64-gnu.cuda-11.3.cudnn8.2.tar.gz
     NVIDIA_VFX_SDK_Ubuntu18.04_0.6.5.0.tgz 
```

# Examples

## Inspect plugin
```shell
gst-inspect-1.0 nvmaxinevideofx
```

## Simple pipeline
```shell
gst-launch-1.0 filesrc location="example_input.mp4" ! \
qtdemux ! avdec_h264 ! queue ! \
nvmaxinevideofx effect=SuperRes modeldir="/usr/local/VideoFX/lib/models" strength=1 upscalefactor=2 ! queue ! \
x264enc ! qtmux ! filesink location="example_output.mp4"
```

## Multiple effect on same pipeline
```shell
gst-launch-1.0 filesrc location="example_input.mp4" ! \
qtdemux ! avdec_h264 ! queue ! \
nvmaxinevideofx effect=ArtifactReduction modeldir="/usr/local/VideoFX/lib/models" strength=1 ! queue ! \
nvmaxinevideofx effect=Denoising modeldir="/usr/local/VideoFX/lib/models" strength=1 ! queue ! \
nvmaxinevideofx effect=SuperRes modeldir="/usr/local/VideoFX/lib/models" strength=1 upscalefactor=2 ! queue ! \
x264enc ! qtmux ! filesink location="example_output.mp4"
```

## Using GreenScreen
GreenScreen filter append foreground mask to buffer metadata using **gstnvmaxinemeta**. You must
provide metadata=1 to BackgroundBlur or Composition if these effect are used after GreenScreen.
```shell
gst-launch-1.0 filesrc location="example_input.mp4" ! \
qtdemux ! avdec_h264 ! queue ! \
nvmaxinevideofx effect=GreenScreen modeldir="/usr/local/VideoFX/lib/models" strength=0 ! queue ! \
nvmaxinevideofx effect=BackgroundBlur modeldir="/usr/local/VideoFX/lib/models" strength=1.0 metadata=1 ! queue ! \
x264enc ! qtmux ! filesink location="example_output.mp4"

gst-launch-1.0 filesrc location="example_input.mp4" ! \
qtdemux ! avdec_h264 ! queue ! \
nvmaxinevideofx effect=GreenScreen modeldir="/usr/local/VideoFX/lib/models" strength=0 ! queue ! \
nvmaxinevideofx effect=Composition modeldir="/usr/local/VideoFX/lib/models" strength=1.0 metadata=1 imagepath="example_bg.jpeg" ! queue ! \
x264enc ! qtmux ! filesink location="example_output.mp4"
```

## Standalone BackgroundBlur and Composition
Both effects have standalone mode in which GreenScreen is used internally. 
```bash
gst-launch-1.0 filesrc location="example_input.mp4" ! \
qtdemux ! avdec_h264 ! queue ! \
nvmaxinevideofx effect=BackgroundBlur modeldir="/usr/local/VideoFX/lib/models" strength=1.0 ! queue ! \
x264enc ! qtmux ! filesink location="example_output.mp4"

gst-launch-1.0 filesrc location="example_input.mp4" ! \
qtdemux ! avdec_h264 ! queue ! \
nvmaxinevideofx effect=Composition modeldir="/usr/local/VideoFX/lib/models" strength=1.0 imagepath="example_bg.jpeg" ! queue ! \
x264enc ! qtmux ! filesink location="example_output.mp4"
```

# Work in progress
1. Create nvmaxineaudiofx plugin based on Maxine<sup>TM</sup> Audio Effects SDK.
2. Create nvmaxinearfx plugin base on Maxine<sup>TM</sup> Augmentend Reality SDK.