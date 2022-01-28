#!/bin/bash
#cd COMPILER_DIR
#./project_init_scripts/init_llvm4.sh

set -e
set -x
COMPILER_DIR=$(readlink -f `dirname $0`/..)
PROJECT_ROOT_DIR=`dirname ${COMPILER_DIR}`
LLVM4_DIR=${PROJECT_ROOT_DIR}/llvm4/llvm-release_40
CLANG4_DIR=${PROJECT_ROOT_DIR}/llvm4/clang-release_40
SYM_LINK='ln -sfn'

#################################  SETUP LLVM  #################################
# All paths used in this explaination are relative to the root dir of the repo
# Uses Release 40 of LLVM an Clang (Version 4.0) that was downloaded from then
# Github mirror.  Extracts these archives to ../llvm/llvm-release_40 and
# ../llvm/clang-release_40.  Then uses setup_symlinks, to patch in the HEXBOX
# changes. 
################################################################################
if [ ! -e ${LLVM4_DIR} ]
then

  mkdir -p ${PROJECT_ROOT_DIR}/llvm4/build
  #  SYM_LINK in clang
  unzip -o ${COMPILER_DIR}/3rd_party/llvm-release_40.zip -d ${COMPILER_DIR}/../llvm4/

fi

if [ ! -e ${CLANG4_DIR} ]
then
  unzip -o ${COMPILER_DIR}/3rd_party/clang-release_40.zip -d ${COMPILER_DIR}/../llvm4/
  ${SYM_LINK} ${CLANG4_DIR} ${LLVM4_DIR}/tools/clang
fi

${COMPILER_DIR}/llvm/setup_symlinks.sh

SCRIPT_DIR=`dirname \`readlink -f $0\``
mkdir -p ${SCRIPT_DIR}/../../llvm4/build
cd ${SCRIPT_DIR}/../../llvm4/build
cmake ../llvm-release_40 -GNinja \
       -DCMAKE_BUILD_TYPE=Debug \
       -DCMAKE_C_COMPILER=clang \
       -DCMAKE_CXX_COMPILER=clang++ \
       -DLLVM_ENABLE_ASSERTIONS=ON  \
       -DLLVM_BUILD_TESTS=OFF  \
       -DLLVM_BUILD_EXAMPLES=OFF  \
       -DLLVM_INCLUDE_TESTS=OFF \
       -DLLVM_INCLUDE_EXAMPLES=OFF \
       -DLLVM_TARGETS_TO_BUILD="X86;ARM" \
       -DBUILD_SHARED_LIBS=ON  \
       -DLLVM_BINUTILS_INCDIR=${PROJECT_ROOT_DIR}/gcc/gcc-arm-none-eabi-6-2017-q1-update/src/binutils/include \
       -DCMAKE_INSTALL_PREFIX=../bin \
       -DCMAKE_C_FLAGS=-fstandalone-debug \
       -DCMAKE_CXX_FLAGS=-fstandalone-debug \
       -DCMAKE_INSTALL_PREFIX=../bins 
ninja 
ninja install
###############################################################################
