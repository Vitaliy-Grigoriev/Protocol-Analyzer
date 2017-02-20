#!/usr/bin/env bash

mkdir check && cd check
cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -j4 ..
oclint-json-compilation-database -v
cd .. && rm -rf check