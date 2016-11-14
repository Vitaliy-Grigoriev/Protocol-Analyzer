#!/usr/bin/env bash

mkdir build && cd build && cmake -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_BUILD_TYPE=Debug -j4 .. && make -j4
cd .. && rm -rf build
