#!/usr/bin/env bash

mkdir check && cd check && cmake -j 4 .. && scan-build make -j 4
cd .. && rm -rf check
