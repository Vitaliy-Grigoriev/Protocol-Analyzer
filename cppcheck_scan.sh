#!/usr/bin/env bash

mkdir check && cd check
cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -j 4 ..
cppcheck -q --language=c++ --template=vs -I/usr/include/ -I/usr/include/c++/6/ -I../include/ --std=c++11 --enable=all --project=compile_commands.json
cd .. && rm -rf check