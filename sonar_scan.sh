#!/usr/bin/env bash

if [ "$#" -lt 1 ]
then
 echo "Error in input parameters!  Not find the version of the build."
 exit "0"
fi

PROJECT_DIR=$(pwd)

mkdir check && cd check

export CXX="/usr/bin/clang++"
export BUILD_TYPE="Debug"
cmake -j4 ..


clang-tidy ../src/framework/*.cpp ../include/framework/*.hpp -checks=*,\
-hicpp-use-auto,\
-modernize-use-auto,\
-google-default-arguments,\
-modernize-redundant-void-arg,\
-llvm-include-order,\
-readability-simplify-boolean-expr,\
-fuchsia-overloaded-operator,\
-fuchsia-default-arguments,\
-google-readability-namespace-comments,\
-llvm-namespace-comment,\
-hicpp-no-array-decay,\
-llvm-header-guard,\
-hicpp-signed-bitwise,\
-cppcoreguidelines-pro-type-reinterpret-cast,\
-cppcoreguidelines-pro-type-const-cast,\
-clang-diagnostic-deprecated-declarations,\
-cppcoreguidelines-pro-bounds-pointer-arithmetic,\
-cppcoreguidelines-pro-bounds-constant-array-index,\
-cppcoreguidelines-pro-bounds-array-to-pointer-decay,\
-cppcoreguidelines-pro-type-vararg,\
-google-runtime-references,\
-misc-misplaced-widening-cast\
 -- -std=c++17 > clangTidyReport.txt


build-wrapper-linux-x86-64 --out-dir output make -j4

sonar-scanner \
  -Dsonar.projectKey=Protocol-Analyzer \
  -Dsonar.projectName="Protocol Analyzer" \
  -Dsonar.projectVersion="$1" \
  -Dsonar.cfamily.threads=3 \
  -Dsonar.sourceEncoding=UTF-8 \
  -Dsonar.language=cpp \
  -Dsonar.cpp.std=c++17 \
  -Dsonar.cxx.clangtidy.reportPath=clangTidyReport.txt \
  -Dsonar.issuesReport.html.enable=true \
  -Dsonar.projectBaseDir=${PROJECT_DIR} \
  -Dsonar.sources=src/framework,include/framework,test \
  -Dsonar.cfamily.build-wrapper-output=output \
  -Dsonar.host.url=https://sonarcloud.io \
  -Dsonar.organization=vitaliy-grigoriev-github \
  -Dsonar.login=d421338a22460d731e2db7b8f7af57647547f8ee

cd .. && rm -rf check