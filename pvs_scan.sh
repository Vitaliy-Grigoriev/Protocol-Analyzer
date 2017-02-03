#!/usr/bin/env bash

mkdir check && cd check
cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=On -j4 ..
pvs-studio-analyzer analyze -o $(pwd)/tmp.log -e /usr/include/ -j4
plog-converter -a "GA;64;OP;CS" -t errorfile $(pwd)/tmp.log
cd .. && rm -rf check
