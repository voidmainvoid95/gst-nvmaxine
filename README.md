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
    libgstreamer-plugins-bad1.0-dev \
    libjpeg-dev
  ```
### Nvidia dependencies
- **cuda-11.8.0**
- **videofx-sdk-linux_0.7.2.0.tgz** \
https://catalog.ngc.nvidia.com/orgs/nvidia/teams/maxine/resources/maxine_linux_video_effects_sdk_ga/version
- **cudnn-linux-x86_64-8.6.0.163_cuda11-archive.tar.xz** \
  https://developer.nvidia.com/compute/cudnn/secure/8.6.0/local_installers/11.8/cudnn-linux-x86_64-8.6.0.163_cuda11-archive.tar.xz
- **TensorRT-8.5.1.7.Linux.x86_64-gnu.cuda-11.8.cudnn8.6.tar.gz** \
  https://developer.nvidia.com/compute/machine-learning/tensorrt/secure/8.5.1/tars/TensorRT-8.5.1.7.Linux.x86_64-gnu.cuda-11.8.cudnn8.6.tar.gz

```bash
# install cuda 11.8.0
wget https://developer.download.nvidia.com/compute/cuda/repos/ubuntu2004/x86_64/cuda-ubuntu2004.pin
sudo mv cuda-ubuntu2004.pin /etc/apt/preferences.d/cuda-repository-pin-600
wget https://developer.download.nvidia.com/compute/cuda/11.8.0/local_installers/cuda-repo-ubuntu2004-11-8-local_11.8.0-520.61.05-1_amd64.deb
sudo dpkg -i cuda-repo-ubuntu2004-11-8-local_11.8.0-520.61.05-1_amd64.deb
sudo cp /var/cuda-repo-ubuntu2004-11-8-local/cuda-*-keyring.gpg /usr/share/keyrings/
sudo apt-get update
sudo apt-get -y install cuda

# install cudnn
tar -xvf cudnn-linux-x86_64-8.6.0.163_cuda11-archive.tar.xz /tmp
sudo mv /tmp/cudnn-linux-x86_64-8.6.0.163_cuda11-archive/LICENSE /usr/local/cuda
sudo mv /tmp/cudnn-linux-x86_64-8.6.0.163_cuda11-archive/include/* /usr/local/cuda/include
sudo mv /tmp/cudnn-linux-x86_64-8.6.0.163_cuda11-archive/lib/* /usr/local/cuda/lib64
rm -rf /tmp/cudnn-linux-x86_64-8.6.0.163_cuda11-archive

# install TensorRT
sudo tar -xvf TensorRT-8.5.1.7.Linux.x86_64-gnu.cuda-11.8.cudnn8.6.tar.gz /usr/local

# install NVIDIA VFK
sudo tar -xvf videofx-sdk-linux_0.7.2.0.tgz -C /usr/local

sudo echo 'export LD_LIBRARY_PATH="${LD_LIBRARY_PATH}:/usr/local/cuda/lib64:/usr/local/VideoFX/lib:/usr/local/TensorRT-8.5.1.7/lib"' >> /etc/profile
```
    

### Ubuntu 20.04 WSL
The plugin also works on Windows using Ubuntu 20.04 WSL. The requirements are the same as normal Ubuntu, just pay attention on using the 
correct version of cuda compatible with WSL. The other steps are the same as normal Ubuntu installation.
```bash
# install cuda 11.8.0 WSL
wget https://developer.download.nvidia.com/compute/cuda/repos/wsl-ubuntu/x86_64/cuda-wsl-ubuntu.pin
sudo mv cuda-wsl-ubuntu.pin /etc/apt/preferences.d/cuda-repository-pin-600
wget https://developer.download.nvidia.com/compute/cuda/11.8.0/local_installers/cuda-repo-wsl-ubuntu-11-8-local_11.8.0-1_amd64.deb
sudo dpkg -i cuda-repo-wsl-ubuntu-11-8-local_11.8.0-1_amd64.deb
sudo cp /var/cuda-repo-wsl-ubuntu-11-6-local/cuda-*-keyring.gpg /usr/share/keyrings/
sudo apt-get update
sudo apt-get -y install cuda
```
More info at https://docs.nvidia.com/cuda/wsl-user-guide/index.html

## Docker
This repo provide a Dockerfile for development and testing. To build image you must
provide:
- videofx-sdk-linux_0.7.2.0.tgz


## Windows

### Base dependencies
- **Microsoft Visual Studio 2019 or later** \
This is the required version from Maxine documentation. The plugin was ported to windows using Visual Studio 2022.
- **cmake 3.25.1** \
https://github.com/Kitware/CMake/releases/download/v3.25.1/cmake-3.25.1-windows-x86_64.msi
- **GStreamer MSVC**
  https://gstreamer.freedesktop.org/download/ \
You need both runtime and development packages
- **libjpeg-turbo-2.x.x MSVC**
  https://sourceforge.net/projects/libjpeg-turbo/files/2.1.4/libjpeg-turbo-2.1.4-vc64.exe/download
### Nvidia dependencies
- **cuda 11.8.0** 
  - Windows 11 \
  https://developer.download.nvidia.com/compute/cuda/11.8.0/local_installers/cuda_11.8.0_522.06_windows.exe
  - Windows 10 \
  https://developer.download.nvidia.com/compute/cuda/11.8.0/local_installers/cuda_11.8.0_522.06_windows.exe
- **NVIDIA Video Effects SDK_win_0.7.2.0.zip** development SDK 
https://catalog.ngc.nvidia.com/orgs/nvidia/teams/maxine/resources/maxine_windows_video_effects_sdk_ga/version \
Install in **C:\\Program Files\\NVIDIA Corporation\\NVIDIA Video Effects\\**.

Add these variables to system environment variable:
- **GSTREAMER_1_0_ROOT_MSVC_X86_64** Pointing to gstreamer installation folder
- **LIB_JPEG** Pointing to libjpeg-turbo installation folder
- **NV_VIDEO_EFFECTS_PATH** Pointing to maxine VideoFX SDK installation folder

Append these to **Path** system variable:
- **%GSTREAMER_1_0_ROOT_MSVC_X86_64%\\bin**
- **%GSTREAMER_1_0_ROOT_MSVC_X86_64%\\lib**
- **%LIB_JPEG%\\bin**
- **%LIB_JPEG%\\lib**

## Build
You can specify the installation directory of the libraries using following cmake variables:
- Windows
  - **GSTREAMER_DIR** default: C:\\gstreamer\\1.0\\msvc_x86_64
  - **NVMAXINE_DIR** default C:\\Program Files\\NVIDIA Corporation\\NVIDIA Video Effects
  - **GLIB_DIR** default: C:\\gstreamer\\1.0\\msvc_x86_64
  - **JPEG_DIR** default: C:\\libjpeg-turbo64
- Ubuntu 20.04
  - **TENSORRT_DIR** default: /usr/local/TensorRT-8.5.1.7

```bash
mkdir build 
cd build
cmake .. 

# For Debug build
cmake --build . --config Debug
cmake --install  . --config Debug

# For Release build
cmake --build . --config Release
cmake --install . --config Release
```

# Examples

## Inspect plugin
```shell
gst-inspect-1.0 nvmaxinevideofx
```

## Simple pipeline
```shell
gst-launch-1.0 filesrc location="example_video.mp4" ! \
qtdemux ! avdec_h264 ! queue ! \
nvmaxinevideofx effect=SuperRes modeldir="/usr/local/VideoFX/lib/models" mode=1 upscalefactor=2 ! queue ! \
x264enc ! qtmux ! filesink location="example_video_output.mp4"
```

## Multiple effect on same pipeline
```shell
gst-launch-1.0 filesrc location="example_video.mp4" ! \
qtdemux ! avdec_h264 ! queue ! \
nvmaxinevideofx effect=ArtifactReduction modeldir="/usr/local/VideoFX/lib/models" mode=1 ! queue ! \
nvmaxinevideofx effect=Denoising modeldir="/usr/local/VideoFX/lib/models" strength=1 ! queue ! \
nvmaxinevideofx effect=SuperRes modeldir="/usr/local/VideoFX/lib/models" mode=1 upscalefactor=2 ! queue ! \
x264enc ! qtmux ! filesink location="example_video_output.mp4"
```

## Using GreenScreen
GreenScreen filter append foreground mask to buffer metadata using **gstnvmaxinemeta**. You must
provide metadata=1 to BackgroundBlur or Composition if these effect are used after GreenScreen.
```shell
gst-launch-1.0 filesrc location="example_video.mp4" ! \
qtdemux ! avdec_h264 ! queue ! \
nvmaxinevideofx effect=GreenScreen modeldir="/usr/local/VideoFX/lib/models" mode=0 ! queue ! \
nvmaxinevideofx effect=BackgroundBlur modeldir="/usr/local/VideoFX/lib/models" strength=1.0 metadata=1 ! queue ! \
x264enc ! qtmux ! filesink location="example_video_output.mp4"

gst-launch-1.0 filesrc location="example_video.mp4" ! \
qtdemux ! avdec_h264 ! queue ! \
nvmaxinevideofx effect=GreenScreen modeldir="/usr/local/VideoFX/lib/models" mode=0 ! queue ! \
nvmaxinevideofx effect=Composition modeldir="/usr/local/VideoFX/lib/models" strength=1.0 metadata=1 imagepath="example_bg.jpeg" ! queue ! \
x264enc ! qtmux ! filesink location="example_video_output.mp4"
```

## Standalone BackgroundBlur and Composition
Both effects have standalone mode in which GreenScreen is used internally. 
```bash
gst-launch-1.0 filesrc location="example_video.mp4" ! \
qtdemux ! avdec_h264 ! queue ! \
nvmaxinevideofx effect=BackgroundBlur modeldir="/usr/local/VideoFX/lib/models" strength=1.0 ! queue ! \
x264enc ! qtmux ! filesink location="example_video_output.mp4"

gst-launch-1.0 filesrc location="example_video.mp4" ! \
qtdemux ! avdec_h264 ! queue ! \
nvmaxinevideofx effect=Composition modeldir="/usr/local/VideoFX/lib/models" strength=1.0 imagepath="example_bg.jpeg" ! queue ! \
x264enc ! qtmux ! filesink location="example_video_output.mp4"
```

# Work in progress
1. Create nvmaxineaudiofx plugin based on Maxine<sup>TM</sup> Audio Effects SDK.
2. Create nvmaxinearfx plugin base on Maxine<sup>TM</sup> Augmentend Reality SDK.