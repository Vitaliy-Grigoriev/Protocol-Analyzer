#!/usr/bin/env bash

PROJECT_DIR=$(pwd)

mkdir check && cd check

export COMPILER="/usr/bin/clang++"
export BUILD_TYPE="Debug"
cmake -j4 ..

build-wrapper-linux-x86-64 --out-dir output make -j4

sonar-scanner \
  -Dsonar.projectKey=Protocol-Analyzer \
  -Dsonar.projectName="Protocol Analyzer" \
  -Dsonar.projectVersion=0.0.8 \
  -Dsonar.organization=vitaliy-grigoriev-github \
  -Dsonar.cfamily.threads=3 \
  -Dsonar.sourceEncoding=UTF-8 \
  -Dsonar.language=cpp \
  -Dsonar.cpp.std=c++17 \
  -Dsonar.issuesReport.html.enable=true \
  -Dsonar.projectBaseDir=${PROJECT_DIR} \
  -Dsonar.sources=src,include/analyzer \
  -Dsonar.cfamily.build-wrapper-output=output \
  -Dsonar.host.url=https://sonarcloud.io \
  -Dsonar.login=d421338a22460d731e2db7b8f7af57647547f8ee

cd .. && rm -rf check