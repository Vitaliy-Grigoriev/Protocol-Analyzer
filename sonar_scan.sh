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


clang-tidy ../src/framework/*.cpp ../include/framework/*.hpp ../src/scanner/*.cpp ../include/scanner/*.hpp -checks=*,\
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
 -- -std=c++17 > clangTidyReport


scan-build --use-analyzer=${CXX} --show-description -analyze-headers \
 -enable-checker alpha.clone.CloneChecker \
 -enable-checker alpha.core.BoolAssignment \
 -enable-checker alpha.core.CallAndMessageUnInitRefArg \
 -enable-checker alpha.core.CastSize \
 -enable-checker alpha.core.CastToStruct \
 -enable-checker alpha.core.Conversion \
 -enable-checker alpha.core.DynamicTypeChecker \
 -enable-checker alpha.core.FixedAddr \
 -enable-checker alpha.core.IdenticalExpr \
 -enable-checker alpha.core.PointerArithm \
 -enable-checker alpha.core.PointerSub \
 -enable-checker alpha.core.SizeofPtr \
 -enable-checker alpha.core.StackAddressAsyncEscape \
 -enable-checker alpha.core.TestAfterDivZero \
 -enable-checker alpha.cplusplus.DeleteWithNonVirtualDtor \
 -enable-checker alpha.cplusplus.IteratorRange \
 -enable-checker alpha.cplusplus.MisusedMovedObject \
 -enable-checker alpha.cplusplus.UninitializedObject \
 -enable-checker alpha.deadcode.UnreachableCode \
 -enable-checker alpha.osx.cocoa.DirectIvarAssignment \
 -enable-checker alpha.osx.cocoa.DirectIvarAssignmentForAnnotatedFunctions \
 -enable-checker alpha.osx.cocoa.InstanceVariableInvalidation \
 -enable-checker alpha.osx.cocoa.MissingInvalidationMethod \
 -enable-checker alpha.osx.cocoa.localizability.PluralMisuseChecker \
 -enable-checker alpha.security.ArrayBoundV2 \
 -enable-checker alpha.security.MallocOverflow \
 -enable-checker alpha.security.MmapWriteExec \
 -enable-checker alpha.security.ReturnPtrRange \
 -enable-checker alpha.security.taint.TaintPropagation \
 -enable-checker alpha.unix.BlockInCriticalSection \
 -enable-checker alpha.unix.Chroot \
 -enable-checker alpha.unix.PthreadLock \
 -enable-checker alpha.unix.SimpleStream \
 -enable-checker alpha.unix.Stream \
 -enable-checker alpha.unix.cstring.BufferOverlap \
 -enable-checker alpha.unix.cstring.NotNullTerminated \
 -enable-checker alpha.unix.cstring.OutOfBounds \
 -plist -o analyzer_reports make -j4


build-wrapper-linux-x86-64 --out-dir output make -j4

sonar-scanner \
  -Dsonar.projectKey=Protocol-Analyzer \
  -Dsonar.projectName="Protocol Analyzer" \
  -Dsonar.projectVersion="$1" \
  -Dsonar.cfamily.threads=3 \
  -Dsonar.sourceEncoding=UTF-8 \
  -Dsonar.language=cpp \
  -Dsonar.cpp.std=c++17 \
  -Dsonar.cxx.clangtidy.reportPath=clangTidyReport \
  -Dsonar.cxx.clangsa.reportPath=analyzer_reports/*/*.plist \
  -Dsonar.issuesReport.html.enable=true \
  -Dsonar.projectBaseDir=${PROJECT_DIR} \
  -Dsonar.sources=src/framework,include/framework,src/scanner,include/scanner,test \
  -Dsonar.cfamily.build-wrapper-output=output \
  -Dsonar.host.url=https://sonarcloud.io \
  -Dsonar.organization=vitaliy-grigoriev-github \
  -Dsonar.login=d421338a22460d731e2db7b8f7af57647547f8ee

cd .. && rm -rf check