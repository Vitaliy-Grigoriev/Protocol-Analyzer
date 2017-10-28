#!/usr/bin/env bash

mkdir build && cd build && cmake -j 4 .. && make -j 4
cd .. && rm -rf build