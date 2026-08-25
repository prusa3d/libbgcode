FROM mcr.microsoft.com/devcontainers/cpp:1-ubuntu-22.04

# Install additional packages for building libbgcode, python bindings, and dependencies
RUN apt-get update && export DEBIAN_FRONTEND=noninteractive \
    && apt-get -y install --no-install-recommends \
        build-essential \
        cmake \
        ninja-build \
        gdb \
        git \
        curl \
        zip \
        unzip \
        tar \
        pkg-config \
        python3 \
        python3-pip \
        python3-venv \
        python3-dev \
    && apt-get autoremove -y && apt-get clean -y && rm -rf /var/lib/apt/lists/*

# Install python build dependencies
RUN python3 -m pip install --no-cache-dir --upgrade pip \
    && python3 -m pip install --no-cache-dir \
        build \
        py-build-cmake \
        pytest

