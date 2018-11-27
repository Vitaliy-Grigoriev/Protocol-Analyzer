#!/usr/bin/env bash

echo "Compiler: ${CXX}."
echo "Build type: ${BUILD_TYPE}."

cmake -j 4 .
if [ $? -ne 0 ]; then exit -1; fi

make -j 4
if [ $? -ne 0 ]; then exit -2; fi

exit 0