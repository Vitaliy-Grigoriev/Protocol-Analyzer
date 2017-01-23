#!/usr/bin/env bash

if [ $# -lt 1 ]
then
 echo "Error in input parameters."
 exit 0
fi

if [ $1 -eq 1 ]
then
clang-tidy src/Log.cpp -checks=*,-clang-analyzer-alpha.*,-google-default-arguments -- -std=c++14 -Iinclude -I/usr/include
exit 0
fi

if [ $1 -eq 2 ]
then
clang-tidy src/Socket.cpp -checks=*,-clang-analyzer-alpha.*,-google-default-arguments -- -std=c++14 -Iinclude -I/usr/include
exit 0
fi

if [ $1 -eq 3 ]
then
clang-tidy src/SocketSSL.cpp -checks=*,-clang-analyzer-alpha.*,-google-default-arguments -- -std=c++14 -Iinclude -I/usr/include
exit 0
fi

if [ $1 -eq 4 ]
then
clang-tidy src/SocketManager.cpp -checks=*,-clang-analyzer-alpha.*,-google-default-arguments -- -std=c++14 -Iinclude -I/usr/include
exit 0
fi