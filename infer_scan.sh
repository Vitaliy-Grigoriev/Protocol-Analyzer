#!/usr/bin/env bash

export CXX="/usr/bin/clang++"
export BUILD_TYPE="Debug"


mkdir check && cd check

cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -j4 ..
infer run --compilation-database compile_commands.json

cd .. && rm -rf check