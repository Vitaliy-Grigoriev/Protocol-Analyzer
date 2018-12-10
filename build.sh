#!/usr/bin/env bash

#export CXX=/usr/bin/g++
#export CXX=/usr/bin/icc
#export CXX=/usr/bin/clang++

#export BUILD_TYPE=Debug
#export BUILD_TYPE=Release

mkdir build && cd build && cmake -j 4 .. && make -j 4
cd .. && rm -rf build
