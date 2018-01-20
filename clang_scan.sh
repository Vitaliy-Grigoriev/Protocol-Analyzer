#!/usr/bin/env bash

export COMPILER="/usr/bin/clang++"
export BUILD_TYPE="Debug"

mkdir check && cd check && cmake -j 4 ..

scan-build --use-analyzer=/usr/bin/clang++ --show-description -analyze-headers --view make -j 4

cd .. && rm -rf check
