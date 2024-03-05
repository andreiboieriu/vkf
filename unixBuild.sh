#!/usr/bin/env bash

CC=$(which clang)
CXX=$(which clang++)

mkdir -p build
cd build
cmake -D CMAKE_C_COMPILER=clang -D CMAKE_CXX_COMPILER=clang++ -DCMAKE_EXPORT_COMPILE_COMMANDS=1 ..
make && make Shaders && ./VulkanEngine
cd ..
