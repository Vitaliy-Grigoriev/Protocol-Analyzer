#!/usr/bin/env bash

if [ "$#" -lt 1 ]
then
 echo "Error in input parameters!  Not find the version of the build."
 exit "0"
fi

mkdir check && cd check && cmake -j 4 ..
/usr/local/bin/cov-build --dir cov-int make -j 4 && tar czvf project.tgz cov-int && curl \
  --form token=7wpvlRSdjmG7H2W5WL0fHw \
  --form email=Vit.link420@gmail.com \
  --form file=@project.tgz \
  --form version="$1" \
  --form description="$2" \
  https://scan.coverity.com/builds?project=Vitaliy-Grigoriev%2FProtocol-Analyzer && echo "Send packet success."
cd .. && rm -rf check
