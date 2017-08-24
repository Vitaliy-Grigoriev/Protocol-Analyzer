#!/usr/bin/env bash

mkdir check && cd check
cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=On -j4 ..
pvs-studio-analyzer analyze -o $(pwd)/tmp.plog -e /usr/include/ -j4
plog-converter -a "GA:1,2,3;64:1,2,3;OP:1,2,3;CS:1,2,3" -t errorfile $(pwd)/tmp.plog
cd .. && rm -rf check
