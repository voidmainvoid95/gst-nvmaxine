FROM nvidia/cuda:11.3.1-devel-ubuntu18.04

COPY cudnn-11.3-linux-x64-v8.2.1.32.tgz /cudnn.tgz

COPY TensorRT-8.0.1.6.Linux.x86_64-gnu.cuda-11.3.cudnn8.2.tar.gz /tensor_rt.tar.gz

COPY NVIDIA_VFX_SDK_Ubuntu18.04_0.6.5.0.tgz /video_fx.tar.gz

RUN tar -xvf /cudnn.tgz -C /usr/local

RUN tar -xvf /tensor_rt.tar.gz -C /usr/local

RUN tar -xvf /video_fx.tar.gz -C /usr/local

RUN rm -rf /cudnn.tgz /tensor_rt.tar.gz /video_fx.tar.gz

RUN apt-get update && apt-get install -y cmake

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get -y install libgstreamer1.0-0 \
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
                        build-essential \
                        meson \
                        ninja-build \
                        cmake \
                        gcc \
                        g++ \
                        gdb \
                        clang \
                        ssh \
                        vim \
                        rsync \
                        gdb \
                        less

RUN useradd -m user && yes password | passwd user

ENV LD_LIBRARY_PATH="${LD_LIBRARY_PATH}:/usr/local/cuda/lib64:/usr/local/VideoFX/lib:/usr/local/TensorRT-8.0.1.6/lib"

RUN echo 'export LD_LIBRARY_PATH="${LD_LIBRARY_PATH}:/usr/local/cuda/lib64:/usr/local/VideoFX/lib:/usr/local/TensorRT-8.0.1.6/lib"' >> /etc/profile

# Enable this entrypoint to use container in ide
ENTRYPOINT service ssh restart && sleep infinity
