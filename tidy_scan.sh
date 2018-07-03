#!/usr/bin/env bash

clang-tidy src/*.cpp include/analyzer/*.hpp -checks=*,\
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
 -- -std=c++17
