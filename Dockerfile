# Use the existing image
FROM ubuntu:20.04

# Install necessary packages: build-essential, cmake, protobuf, gRPC, GTest, and dependencies
RUN apt-get update && DEBIAN_FRONTEND=noninteractive apt-get install -y \
    build-essential \
    cmake \
    libgtest-dev \
    g++ \
    make \
    libgmp-dev \
    curl \
    libprotobuf-dev \
    protobuf-compiler \
    git \
    libtool \
    autoconf \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

# Clone gRPC and install it
RUN git clone -b v1.41.1 --recursive https://github.com/grpc/grpc /grpc && \
    cd /grpc && \
    mkdir -p cmake/build && cd cmake/build && \
    cmake ../.. && \
    make && make install

# Install protobuf
RUN git clone https://github.com/protocolbuffers/protobuf.git && \
    cd protobuf && \
    git checkout v3.18.1 && \
    git submodule update --init --recursive && \
    ./autogen.sh && \
    ./configure && \
    make && \
    make install && \
    ldconfig

# Build and install Google Test
RUN mkdir -p /usr/src/gtest/build \
    && cd /usr/src/gtest/build \
    && cmake .. \
    && make \
    && cp lib/*.a /usr/lib


# Update the package list
RUN apt-get update

# Install Qt dependencies in one step
RUN DEBIAN_FRONTEND=noninteractive apt-get install -y \
    qtbase5-dev \
    qtdeclarative5-dev \
    qttools5-dev \
    qt5-qmake \
    qttools5-dev-tools \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

# Install additional dependencies in separate steps
RUN DEBIAN_FRONTEND=noninteractive apt-get install -y libqt5printsupport5 \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

# Set environment variables to disable interaction with terminal during installation
ENV DEBIAN_FRONTEND=noninteractive

# Update the package list and install required dependencies including libbson-dev
RUN apt-get update && \
    apt-get install -y libbson-dev && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*

# Update the package list and install pkg-config
RUN apt-get update && \
    apt-get install -y pkg-config && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*
    
RUN apt-get update && \
    apt-get install -y \
    nlohmann-json3-dev && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*

# Install dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    libgtk-3-dev \
    libboost-all-dev \
    libtbb2 \
    libtbb-dev \
    libjpeg-dev \
    libpng-dev \
    libtiff-dev \
    libavcodec-dev \
    libavformat-dev \
    libswscale-dev \
    libv4l-dev \
    libxvidcore-dev \
    libx264-dev

# Clone OpenCV repositories
RUN git clone https://github.com/opencv/opencv.git && \
    git clone https://github.com/opencv/opencv_contrib.git

# Checkout the specific version 4.2.0 (to match what you're using)
RUN cd opencv && git checkout 4.2.0 && cd ../opencv_contrib && git checkout 4.2.0

# Build OpenCV with contrib modules
RUN mkdir /opencv/build && cd /opencv/build && \
    cmake -DOPENCV_EXTRA_MODULES_PATH=../../opencv_contrib/modules ../opencv && \
    make -j$(nproc) && \
    make install && \
    ldconfig

# Set working directory
WORKDIR /app

# Copy the CMake project
COPY . .
# Specify command to run the application
CMD ["bash"]