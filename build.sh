#!/usr/bin/env bash

export COMPILER="/usr/bin/clang++"
#export COMPILER="/usr/bin/g++"
#export COMPILER="/opt/intel/bin/icc"
export BUILD_TYPE="Debug"

mkdir build && cd build && cmake -j 4 .. && make -j 4
cd .. && rm -rf build
