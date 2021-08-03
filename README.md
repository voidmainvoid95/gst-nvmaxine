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
Retrieve **VideoFX-ubuntu18.04-x86_64-0.6.0.0.tar.gz** from  https://developer.nvidia.com/maxine-getting-started.
   **README_quickstart.md** present inside archive provides download links for the requested dependencies.

```
     cudnn-11.1-linux-x64-v8.0.4.30.tgz
     TensorRT-7.2.2.3.Ubuntu-18.04.x86_64-gnu.cuda-11.1.cudnn8.0.tar.gz
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
     cudnn-11.1-linux-x64-v8.0.4.30.tgz
     TensorRT-7.2.2.3.Ubuntu-18.04.x86_64-gnu.cuda-11.1.cudnn8.0.tar.gz
     VideoFX-ubuntu18.04-x86_64-0.6.0.0.tar.gz
```

# Examples

## Inspect plugin
```shell
gst-inspect-1.0 nvmaxinevideofx
```

## Simple pipeline
```shell
gst-launch-1.0 filesrc location="example_input.mp4" ! \
qtdemux ! avdec_h264 ! video/x-raw,width=example_width,height=example_height ! queue ! \
nvmaxinevideofx effect=SuperRes modeldir="/usr/local/VideoFx/lib/models" strength=1 upscalefactor=2 ! queue ! \
x264enc ! mp4mux ! filesink location="example_output.mp4"
```

## Multiple effect on same pipeline
```shell
gst-launch-1.0 filesrc location="example_input.mp4" ! \
qtdemux ! avdec_h264 ! video/x-raw,width=example_width,height=example_height ! queue ! \
nvmaxinevideofx effect=ArtifactReduction modeldir="/usr/local/VideoFx/lib/models" strength=1 ! queue ! \
nvmaxinevideofx effect=Denoising modeldir="/usr/local/VideoFx/lib/models" strength=1 ! queue ! \
nvmaxinevideofx effect=SuperRes modeldir="/usr/local/VideoFx/lib/models" strength=1 upscalefactor=2 ! queue ! \
x264enc ! mp4mux ! filesink location="example_output.mp4"
```

## Using GreenScreen
GreenScreen filter append foreground mask to buffer metadata using **gstnvmaxinemeta**. You must
provide metadata=1 to BackgroundBlur or Composition if these effect are used after GreenScreen.
```shell
gst-launch-1.0 filesrc location="example_input.mp4" ! \
qtdemux ! avdec_h264 ! video/x-raw,width=example_width,height=example_height ! queue ! \
nvmaxinevideofx effect=GreenScreen modeldir="/usr/local/VideoFx/lib/models" strength=0 ! queue ! \
nvmaxinevideofx effect=BackgroundBlur modeldir="/usr/local/VideoFx/lib/models" strength=1.0 metadata=1 ! queue ! \
x264enc ! mp4mux ! filesink location="example_output.mp4"

gst-launch-1.0 filesrc location="example_input.mp4" ! \
qtdemux ! avdec_h264 ! video/x-raw,width=example_width,height=example_height ! queue ! \
nvmaxinevideofx effect=GreenScreen modeldir="/usr/local/VideoFx/lib/models" strength=0 ! queue ! \
nvmaxinevideofx effect=Composition modeldir="/usr/local/VideoFx/lib/models" strength=1.0 metadata=1 imagepath="example_bg.jpeg" ! queue ! \
x264enc ! mp4mux ! filesink location="example_output.mp4"
```

## Standalone BackgroundBlur and Composition
Both effects have standalone mode in which GreenScreen is used internally. 
```bash
gst-launch-1.0 filesrc location="example_input.mp4" ! \
qtdemux ! avdec_h264 ! video/x-raw,width=example_width,height=example_height ! queue ! \
nvmaxinevideofx effect=BackgroundBlur modeldir="/usr/local/VideoFx/lib/models" strength=1.0 ! queue ! \
x264enc ! mp4mux ! filesink location="example_output.mp4"

gst-launch-1.0 filesrc location="example_input.mp4" ! \
qtdemux ! avdec_h264 ! video/x-raw,width=example_width,height=example_height ! queue ! \
nvmaxinevideofx effect=Composition modeldir="/usr/local/VideoFx/lib/models" strength=1.0 imagepath="example_bg.jpeg" ! queue ! \
x264enc ! mp4mux ! filesink location="example_output.mp4"
```

# Work in progress
Composition will support generic image or video input as sub-pipeline.
