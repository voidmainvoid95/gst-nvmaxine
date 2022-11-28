FROM nvidia/cuda:11.6.1-devel-ubuntu20.04

COPY cudnn-linux-x86_64-8.4.0.27_cuda11.6-archive.tar.xz /cudnn.tar.xz

COPY TensorRT-8.2.5.1.Linux.x86_64-gnu.cuda-11.4.cudnn8.2.tar.gz /tensor_rt.tar.gz

COPY NVIDIA_VFX_SDK_Linux_0.7.1.0.tgz /video_fx.tar.gz

RUN tar -xvf /cudnn.tar.xz -C /usr/local

RUN tar -xvf /tensor_rt.tar.gz -C /usr/local

RUN tar -xvf /video_fx.tar.gz -C /usr/local

RUN rm -rf /cudnn.tar.xz /tensor_rt.tar.gz /video_fx.tar.gz

ENV DEBIAN_FRONTEND=noninteractive
ENV TZ=Europe/Rome
RUN ln -snf /usr/share/zoneinfo/${TZ} /etc/localtime && echo ${TZ} > /etc/timezone

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

ENV LD_LIBRARY_PATH="${LD_LIBRARY_PATH}:/usr/local/cuda/lib64:/usr/local/VideoFX/lib:/usr/local/TensorRT-8.2.5.1/lib"

RUN echo 'export LD_LIBRARY_PATH="${LD_LIBRARY_PATH}:/usr/local/cuda/lib64:/usr/local/VideoFX/lib:/usr/local/TensorRT-8.2.5.1/lib"' >> /etc/profile

# Enable this entrypoint to use container in ide
ENTRYPOINT service ssh restart && /bin/bash
