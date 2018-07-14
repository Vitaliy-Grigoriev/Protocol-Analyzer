#!/usr/bin/env bash

mkdir check && cd check

export COMPILER="/usr/bin/clang++"
export BUILD_TYPE="Debug"
cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -j4 ..

cppcheck -q --language=c++ --template=vs -I/usr/include/ -I/usr/include/c++/7.2.0 -I../include/framework/ --std=c++14 --enable=all --project=compile_commands.json

cd .. && rm -rf check
