#!/bin/bash

set -e
REPO_ROOT=$(readlink -f `dirname $0`/..)
PROJECT_ROOT_DIR=`dirname ${REPO_ROOT}`

LLVM_DIR=${PROJECT_ROOT_DIR}/llvm4/llvm-release_40
CLANG_DIR=${PROJECT_ROOT_DIR}/llvm4/clang-release_40

SYM_LINK='ln -sfn'

echo $REPO_ROOT
echo $PROJECT_ROOT_DIR
echo $LLVM_DIR
echo $CLANG_DIR

${SYM_LINK} ${CLANG_DIR} ${LLVM_DIR}/tools/clang
${SYM_LINK} ${REPO_ROOT}/llvm/json-include ${LLVM_DIR}/include/llvm/json
${SYM_LINK} ${REPO_ROOT}/llvm/InitializePasses.h ${LLVM_DIR}/include/llvm/InitializePasses.h

${SYM_LINK} ${REPO_ROOT}/llvm/jsoncpp.cpp ${LLVM_DIR}/lib/Support/jsoncpp.cpp
${SYM_LINK} ${REPO_ROOT}/llvm/lib_support_CMakeLists.txt ${LLVM_DIR}/lib/Support/CMakeLists.txt
${SYM_LINK} ${REPO_ROOT}/llvm/lib_transform_utils_CMakeLists.txt ${LLVM_DIR}/lib/Transforms/Utils/CMakeLists.txt
${SYM_LINK} ${REPO_ROOT}/llvm/LinkAllPasses.h ${LLVM_DIR}/include/llvm/LinkAllPasses.h
${SYM_LINK} ${REPO_ROOT}/llvm/OIApplication.cpp ${LLVM_DIR}/lib/Transforms/Utils/OIApplication.cpp
${SYM_LINK} ${REPO_ROOT}/llvm/PassManagerBuilder.cpp ${LLVM_DIR}/lib/Transforms/IPO/PassManagerBuilder.cpp
${SYM_LINK} ${REPO_ROOT}/llvm/Utils.cpp ${LLVM_DIR}/lib/Transforms/Utils/Utils.cpp
${SYM_LINK} ${REPO_ROOT}/llvm/Instrumentation.h ${LLVM_DIR}/include/llvm/Transforms/Instrumentation.h

# backend
${SYM_LINK} ${REPO_ROOT}/llvm/ARM.h ${LLVM_DIR}/lib/Target/ARM/ARM.h
${SYM_LINK} ${REPO_ROOT}/llvm/ARMTargetMachine.cpp ${LLVM_DIR}/lib/Target/ARM/ARMTargetMachine.cpp
${SYM_LINK} ${REPO_ROOT}/llvm/MCprinter.cpp ${LLVM_DIR}/lib/Target/ARM/MCprinter.cpp
${SYM_LINK} ${REPO_ROOT}/llvm/lib-target-arm-cmakelists.txt ${LLVM_DIR}/lib/Target/ARM/CMakeLists.txt

echo Finished
