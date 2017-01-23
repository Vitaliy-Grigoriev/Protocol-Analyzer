#!/usr/bin/env bash

if [ $# -lt 1 ]
then
 echo "Error in input parameters!!!"
 exit 0
fi

mkdir check && cd check && cmake -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_BUILD_TYPE=Debug -j 4 ..
cov-build --dir cov-int make -j 4 && tar czvf myproject.tgz cov-int && curl --form token=7wpvlRSdjmG7H2W5WL0fHw \
  --form email=vit.link420@gmail.com --form file=@myproject.tgz \
  --form version="$1" --form description="$2" \
  https://scan.coverity.com/builds?project=Vitaliy-Grigoriev%2Fhttp2-analyzer && echo "Send packet success."
cd .. && rm -rf check
