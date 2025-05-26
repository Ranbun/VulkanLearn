#!/bin/bash

set -e

BUILD_DIR=build_release

if [ ! -d "$BUILD_DIR" ]; then
    mkdir "$BUILD_DIR"
fi

cmake -DCMAKE_BUILD_TYPE=Release -G Ninja -DGLFW_INSTALL=OFF -S . -B "$BUILD_DIR"
cmake --build "$BUILD_DIR" --target all -j $(nproc)
