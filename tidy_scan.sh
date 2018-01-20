#!/usr/bin/env bash

if [ "$#" -lt 1 ]
then
 clang-tidy src/*.cpp include/analyzer/*.hpp -checks=*,-google-default-arguments,-modernize-redundant-void-arg,-llvm-include-order,-readability-simplify-boolean-expr -- -std=c++17 -Iinclude/analizer -I/usr/include
 exit "0"
fi

if [ "$1" -eq 1 ]
then
clang-tidy src/Log.cpp -checks=*,-clang-analyzer-alpha.*,-google-default-arguments -- -std=c++17 -Iinclude/analizer -I/usr/include
exit "0"
fi

if [ "$1" -eq 2 ]
then
clang-tidy src/Socket.cpp src/SocketSSL.cpp -checks=*,-clang-analyzer-alpha.*,-google-default-arguments -- -std=c++17 -Iinclude/analizer -I/usr/include
exit "0"
fi

if [ "$1" -eq 3 ]
then
clang-tidy src/Task.cpp src/TaskManager.cpp -checks=*,-clang-analyzer-alpha.*,-google-default-arguments -- -std=c++17 -Iinclude/analizer -I/usr/include
exit "0"
fi

if [ "$1" -eq 4 ]
then
clang-tidy src/Common.cpp src/System.cpp src/Mutex.cpp src/Parser.cpp src/Utilities.cpp src/Timer.cpp -checks=*,-clang-analyzer-alpha.*,-google-default-arguments -- -std=c++14 -Iinclude/analizer -I/usr/include
exit "0"
fi

if [ "$1" -eq 5 ]
then
clang-tidy src/Protocol.cpp -checks=*,-clang-analyzer-alpha.*,-google-default-arguments -- -std=c++17 -Iinclude/analizer -I/usr/include
exit "0"
fi
