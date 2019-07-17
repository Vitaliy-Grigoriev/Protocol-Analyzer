#!/usr/bin/env bash

if [ "$#" -lt 1 ]
then
 echo "Error in input parameters!  Not find the version of the build."
 exit "0"
fi

export COMPILER="/usr/bin/g++"
export BUILD_TYPE="Debug"

mkdir check && cd check && cmake -j4 ..

/usr/bin/cov-configure --gcc
/usr/bin/cov-build --dir cov-int make -j4 && tar czvf project.tgz cov-int && curl \
  --form token=7wpvlRSdjmG7H2W5WL0fHw \
  --form email=Vit.link420@gmail.com \
  --form file=@project.tgz \
  --form version="$1" \
  --form description="$2" \
  https://scan.coverity.com/builds?project=Vitaliy-Grigoriev%2FProtocol-Analyzer && echo "Send packet success."

cp cov-int/build-log.txt ../coverity-build-log.txt
cd .. && rm -rf check
