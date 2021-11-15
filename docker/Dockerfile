# Pull base image.

FROM ubuntu:20.04

# Install.
RUN apt-get update
RUN DEBIAN_FRONTEND="noninteractive" \
    apt-get install -y build-essential git cmake g++ gcc wget unzip libeigen3-dev libsleef-dev
RUN wget https://download.pytorch.org/libtorch/cpu/libtorch-cxx11-abi-shared-with-deps-1.10.0%2Bcpu.zip
RUN unzip -a libtorch-cxx11-abi-shared-with-deps-1.10.0+cpu.zip && rm libtorch-cxx11-abi-shared-with-deps-1.10.0+cpu.zip && \
    cd libtorch && \
    cp -r ./* /usr/local/ && \
    cd / && rm -rf /libtorch

