#!/usr/bin/env bash

export CXX="/usr/bin/clang++"
export BUILD_TYPE="Debug"

mkdir check && cd check && cmake -j4 ..

scan-build --use-analyzer=${COMPILER} --show-description -analyze-headers \
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
 -plist make -j4

cd .. && rm -rf check
