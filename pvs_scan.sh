#!/usr/bin/env bash

mkdir check && cd check

export COMPILER="/usr/bin/clang++"
export BUILD_TYPE="Debug"
cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=On -j4 ..

pvs-studio-analyzer analyze -a "29" -r "/" --compiler clang++ -o $(pwd)/tmp.plog -e /usr/include/ -j4
plog-converter -a "GA:1,2,3;64:1,2,3;OP:1,2,3;CS:1,2,3" -d "V112,V122" --renderTypes=html -o "../pvs-result" $(pwd)/tmp.plog

cd .. && rm -rf check
