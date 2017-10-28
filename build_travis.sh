#!/usr/bin/env bash

echo $COMPILER
echo $BUILD_TIPE

mkdir build && cd build && cmake -j 4 .. && make -j 4
cd .. && rm -rf build