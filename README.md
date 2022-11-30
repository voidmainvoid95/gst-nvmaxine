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

Each filter have a limitation regarding the max and min supported resolution. \
Developer's guide is available at https://catalog.ngc.nvidia.com/orgs/nvidia/teams/maxine/resources/maxine_linux_video_effects_sdk_ga

## Extra videofx effects
* Composition

This effect allows you to change the background using foreground mask provided by GreenScreen with a single jpeg.
## Ubuntu 20.04

### Base dependencies
```bash
sudo apt-get update
sudo apt-get install \
    build-essential \
    cmake \
    libgstreamer1.0-0 \
    gstreamer1.0-plugins-base \
    gstreamer1.0-plugins-good \
    gstreamer1.0-plugins-bad \
    gstreamer1.0-plugins-ugly \
    gstreamer1.0-libav \
    gstreamer1.0-doc \
    gstreamer1.0-tools \
    gstreamer1.0-x \
    gstreamer1.0-alsa \
    gstreamer1.0-gl \
    gstreamer1.0-gtk3 \
    gstreamer1.0-qt5 \
    gstreamer1.0-pulseaudio \
    libgstreamer1.0-dev \
    libgstreamer-plugins-base1.0-dev \
    libgstreamer-plugins-good1.0-dev \
    libgstreamer-plugins-bad1.0-dev
  ```
### Nvidia dependencies
- **cuda-11.6.1**
- **NVIDIA_VFX_SDK_Linux_0.7.1.0.tgz** \
https://catalog.ngc.nvidia.com/orgs/nvidia/teams/maxine/resources/maxine_linux_video_effects_sdk_ga/version
- **cudnn-linux-x86_64-8.4.0.27_cuda11.6-archive.tar.xz** \
https://developer.nvidia.com/compute/cudnn/secure/8.4.0/local_installers/11.6/cudnn-linux-x86_64-8.4.0.27_cuda11.6-archive.tar.xz
- **TensorRT-8.2.5.1.Linux.x86_64-gnu.cuda-11.4.cudnn8.2.tar.gz** \
https://developer.nvidia.com/compute/machine-learning/tensorrt/secure/8.2.5.1/tars/tensorrt-8.2.5.1.linux.x86_64-gnu.cuda-11.4.cudnn8.2.tar.gz

```bash
# install cuda 11.6.1
wget https://developer.download.nvidia.com/compute/cuda/repos/ubuntu2004/x86_64/cuda-ubuntu2004.pin
sudo mv cuda-ubuntu2004.pin /etc/apt/preferences.d/cuda-repository-pin-600
wget https://developer.download.nvidia.com/compute/cuda/11.6.1/local_installers/cuda-repo-ubuntu2004-11-6-local_11.6.1-510.47.03-1_amd64.deb
sudo dpkg -i cuda-repo-ubuntu2004-11-6-local_11.6.1-510.47.03-1_amd64.deb
sudo apt-key add /var/cuda-repo-ubuntu2004-11-6-local/7fa2af80.pub
sudo apt-get update
sudo apt-get -y install cuda

# install cudnn
tar -xvf cudnn-linux-x86_64-8.4.0.27_cuda11.6-archive.tar.xz /tmp
sudo mv /tmp/cudnn-linux-x86_64-8.4.0.27_cuda11.6-archive/LICENSE /usr/local/cuda
sudo mv /tmp/cudnn-linux-x86_64-8.4.0.27_cuda11.6-archive/include/* /usr/local/cuda/include
sudo mv /tmp/cudnn-linux-x86_64-8.4.0.27_cuda11.6-archive/lib/* /usr/local/cuda/lib64
rm -rf /tmp/cudnn-linux-x86_64-8.4.0.27_cuda11.6-archive

# install TensorRT
sudo tar -xvf TensorRT-8.2.5.1.Linux.x86_64-gnu.cuda-11.4.cudnn8.2.tar.gz /usr/local

# install NVIDIA VFK
sudo tar -xvf NVIDIA_VFX_SDK_Linux_0.7.1.0.tgz -C /usr/local
```
    

### Ubuntu 20.04 WSL
The plugin also works on Windows using Ubuntu 20.04 WSL. The requirements are the same as normal Ubuntu, just pay attention on using the 
correct version of cuda compatible with WSL.
```bash
# install cuda 11.6.1 WSL
wget https://developer.download.nvidia.com/compute/cuda/repos/wsl-ubuntu/x86_64/cuda-wsl-ubuntu.pin
sudo mv cuda-wsl-ubuntu.pin /etc/apt/preferences.d/cuda-repository-pin-600
wget https://developer.download.nvidia.com/compute/cuda/11.6.1/local_installers/cuda-repo-wsl-ubuntu-11-6-local_11.6.1-1_amd64.deb
sudo dpkg -i cuda-repo-wsl-ubuntu-11-6-local_11.6.1-1_amd64.deb
sudo cp /var/cuda-repo-wsl-ubuntu-11-6-local/cuda-*-keyring.gpg /usr/share/keyrings/
sudo apt-get update
sudo apt-get -y install cuda
```
More info at https://docs.nvidia.com/cuda/wsl-user-guide/index.html

## Docker
This repo provide a Dockerfile for development and testing. To build image you must
provide:
- NVIDIA_VFX_SDK_Linux_0.7.1.0.tgz
- cudnn-linux-x86_64-8.4.0.27_cuda11.6-archive.tar.xz
- TensorRT-8.2.5.1.Linux.x86_64-gnu.cuda-11.4.cudnn8.2.tar.gz

## Build
Meson is currently unsupported use cmake. 

```shell
mkdir build && cd build
cmake .. && make -j$(nproc)
make install
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
nvmaxinevideofx effect=SuperRes modeldir="/usr/local/VideoFX/lib/models" mode=1 upscalefactor=2 ! queue ! \
x264enc ! qtmux ! filesink location="example_output.mp4"
```

## Multiple effect on same pipeline
```shell
gst-launch-1.0 filesrc location="example_input.mp4" ! \
qtdemux ! avdec_h264 ! queue ! \
nvmaxinevideofx effect=ArtifactReduction modeldir="/usr/local/VideoFX/lib/models" mode=1 ! queue ! \
nvmaxinevideofx effect=Denoising modeldir="/usr/local/VideoFX/lib/models" strength=1 ! queue ! \
nvmaxinevideofx effect=SuperRes modeldir="/usr/local/VideoFX/lib/models" mode=1 upscalefactor=2 ! queue ! \
x264enc ! qtmux ! filesink location="example_output.mp4"
```

## Using GreenScreen
GreenScreen filter append foreground mask to buffer metadata using **gstnvmaxinemeta**. You must
provide metadata=1 to BackgroundBlur or Composition if these effect are used after GreenScreen.
```shell
gst-launch-1.0 filesrc location="example_input.mp4" ! \
qtdemux ! avdec_h264 ! queue ! \
nvmaxinevideofx effect=GreenScreen modeldir="/usr/local/VideoFX/lib/models" mode=0 ! queue ! \
nvmaxinevideofx effect=BackgroundBlur modeldir="/usr/local/VideoFX/lib/models" strength=1.0 metadata=1 ! queue ! \
x264enc ! qtmux ! filesink location="example_output.mp4"

gst-launch-1.0 filesrc location="example_input.mp4" ! \
qtdemux ! avdec_h264 ! queue ! \
nvmaxinevideofx effect=GreenScreen modeldir="/usr/local/VideoFX/lib/models" mode=0 ! queue ! \
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