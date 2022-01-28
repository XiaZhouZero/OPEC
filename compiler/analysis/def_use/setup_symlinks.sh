#!/bin/bash

set -e
set -x
REPO_ROOT=$(readlink -f `dirname $0`/../..)
PROJECT_ROOT_DIR=`dirname ${REPO_ROOT}`

LLVM_DIR=${PROJECT_ROOT_DIR}/llvm9/llvm-9.0.0.src

SYM_LINK='ln -sfn'
echo $PROJECT_ROOT_DIR
echo $LLVM_DIR

${SYM_LINK} ${PROJECT_ROOT_DIR}/compiler/analysis/def_use/json-include ${LLVM_DIR}/include/llvm/json
${SYM_LINK} ${PROJECT_ROOT_DIR}/compiler/analysis/def_use/jsoncpp.cpp ${LLVM_DIR}/lib/Support/jsoncpp.cpp
${SYM_LINK} ${PROJECT_ROOT_DIR}/compiler/analysis/def_use/lib_support_CMakeLists.txt ${LLVM_DIR}/lib/Support/CMakeLists.txt

${SYM_LINK} ${PROJECT_ROOT_DIR}/compiler/analysis/def_use/CMakeLists.txt ${LLVM_DIR}/lib/Transforms/Utils/CMakeLists.txt
${SYM_LINK} ${PROJECT_ROOT_DIR}/compiler/analysis/def_use/InitializePasses.h ${LLVM_DIR}/include/llvm/InitializePasses.h
${SYM_LINK} ${PROJECT_ROOT_DIR}/compiler/analysis/def_use/LinkAllPasses.h ${LLVM_DIR}/include/llvm/LinkAllPasses.h
${SYM_LINK} ${PROJECT_ROOT_DIR}/compiler/analysis/def_use/OIAnalysis.cpp ${LLVM_DIR}/lib/Transforms/Utils/OIAnalysis.cpp
${SYM_LINK} ${PROJECT_ROOT_DIR}/compiler/analysis/def_use/PassManagerBuilder.cpp ${LLVM_DIR}/lib/Transforms/IPO/PassManagerBuilder.cpp
${SYM_LINK} ${PROJECT_ROOT_DIR}/compiler/analysis/def_use/Utils.cpp ${LLVM_DIR}/lib/Transforms/Utils/Utils.cpp
${SYM_LINK} ${PROJECT_ROOT_DIR}/compiler/analysis/def_use/Utils.h ${LLVM_DIR}/include/llvm/Transforms/Utils.h



echo Finished
