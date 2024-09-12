# Use an official Ubuntu as a base image
FROM ubuntu:22.04

# Set environment variables to avoid user input prompts during installation
ENV DEBIAN_FRONTEND=noninteractive

# Install required packages and clean up apt cache afterwards
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    libgtest-dev && \
    rm -rf /var/lib/apt/lists/*

# Build and install GoogleTest
RUN cd /usr/src/gtest && \
    cmake . && \
    make && \
    cp lib/libgtest*.a /usr/lib

# Create and set the working directory
WORKDIR /usr/src/myapp

# Copy the current directory contents into the container
COPY . .
# Create the build directory
RUN rm -rf build && mkdir build

# Build the project using CMake
RUN cmake -B build . && cmake --build build

# Run all test executables sequentially
CMD ["bash", "-c", "./build/TestBigInt10 && ./build/TestBigInt64 && ./build/TestRSA"]
