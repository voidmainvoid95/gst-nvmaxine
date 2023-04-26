FROM nvcr.io/nvidia/tensorrt:22.12-py3

COPY videofx-sdk-linux_0.7.2.0.tgz /tmp/video_fx.tar.gz

RUN tar -xvf /tmp/video_fx.tar.gz -C /usr/local && \
    rm -rf /tmp/video_fx.tar.gz

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

ENV LD_LIBRARY_PATH="${LD_LIBRARY_PATH}:/usr/local/cuda/lib64:/usr/local/VideoFX/lib"

RUN echo 'export LD_LIBRARY_PATH="${LD_LIBRARY_PATH}:/usr/local/cuda/lib64:/usr/local/VideoFX/lib"' >> /etc/profile

# Enable this entrypoint to use container in ide
# ENTRYPOINT service ssh restart && /bin/bash
