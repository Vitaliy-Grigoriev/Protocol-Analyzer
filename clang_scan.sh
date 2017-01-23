#!/usr/bin/env bash

mkdir check && cd check && cmake -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_BUILD_TYPE=Debug -j 4 .. && scan-build make -j 4
cd .. && rm -rf check
