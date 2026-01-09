#!/bin/bash

set -e

# TODO - setup a CMake build

for compiler in g++ clang++; do
    if ! command -v $compiler; then
        echo Not compiling with $compiler, program not found
    fi
    for version in 11 14 17 20 ; do
        build_dir=build_${compiler}_${version} 
        mkdir -p $build_dir
        CXX=$compiler CXX_STD=c++${version} build_dir=$build_dir \
            make -j 4 &
    done
done

wait
