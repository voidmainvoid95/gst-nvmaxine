FROM nvidia/cuda:11.1-devel-ubuntu18.04

COPY cudnn-11.1-linux-x64-v8.0.4.30.tgz /cudnn-11.1-linux-x64-v8.0.4.30.tgz

COPY TensorRT-7.2.2.3.Ubuntu-18.04.x86_64-gnu.cuda-11.1.cudnn8.0.tar.gz /TensorRT-7.2.2.3.Ubuntu-18.04.x86_64-gnu.cuda-11.1.cudnn8.0.tar.gz

COPY VideoFX-ubuntu18.04-x86_64-0.6.0.0.tar.gz /VideoFX-ubuntu18.04-x86_64-0.6.0.0.tar.gz

RUN tar -xvf /cudnn-11.1-linux-x64-v8.0.4.30.tgz -C /usr/local

RUN tar -xvf /TensorRT-7.2.2.3.Ubuntu-18.04.x86_64-gnu.cuda-11.1.cudnn8.0.tar.gz -C /usr/local

RUN tar -xvf /VideoFX-ubuntu18.04-x86_64-0.6.0.0.tar.gz -C /usr/local

RUN rm -rf /cudnn-11.1-linux-x64-v8.0.4.30.tgz /TensorRT-7.2.2.3.Ubuntu-18.04.x86_64-gnu.cuda-11.1.cudnn8.0.tar.gz /VideoFX-ubuntu18.04-x86_64-0.6.0.0.tar.gz

RUN apt-get update && apt-get install -y cmake

ENV LD_LIBRARY_PATH="${LD_LIBRARY_PATH}:/usr/local/cuda/lib64:/usr/local/VideoFX/lib:/usr/local/TensorRT-7.2.2.3/lib"

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

RUN echo 'export LD_LIBRARY_PATH="${LD_LIBRARY_PATH}:/usr/local/cuda/lib64:/usr/local/VideoFX/lib:/usr/local/TensorRT-7.2.2.3/lib"' >> /etc/profile

RUN cd /usr/local/VideoFX/share/ && ./build_samples.sh -f -y -c -i ~/maxine-videofx-samples

# Enable this entrypoint to use container in ide
ENTRYPOINT service ssh restart && sleep infinity
