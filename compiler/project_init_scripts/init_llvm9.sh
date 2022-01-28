#!/bin/bash
#cd COMPILER_DIR
#./project_init_scripts/init_llvm9.sh

set -e
set -x
COMPILER_DIR=$(readlink -f `dirname $0`/..)
PROJECT_ROOT_DIR=`dirname ${COMPILER_DIR}`
THIS_DIR=`dirname \`readlink -f $0\``
LLVM9_DIR=${PROJECT_ROOT_DIR}/llvm9/llvm-9.0.0.src
CLANG9_DIR=${PROJECT_ROOT_DIR}/llvm9/cfe-9.0.0.src
SYM_LINK='ln -sfn'

#################################  SETUP LLVM9/CLANG9  #################################
if [ ! -e ${LLVM9_DIR} ]
then
  mkdir -p ${PROJECT_ROOT_DIR}/llvm9
  tar -xvJf ${COMPILER_DIR}/3rd_party/llvm-9.0.0.src.tar.xz -C ${PROJECT_ROOT_DIR}/llvm9
  tar -xvJf ${COMPILER_DIR}/3rd_party/cfe-9.0.0.src.tar.xz -C ${PROJECT_ROOT_DIR}/llvm9
fi

${SYM_LINK} ${CLANG9_DIR} ${LLVM9_DIR}/tools/clang
bash ${PROJECT_ROOT_DIR}/compiler/analysis/def_use/setup_symlinks.sh

mkdir -p ${PROJECT_ROOT_DIR}/llvm9/build
cd ${PROJECT_ROOT_DIR}/llvm9/build

cmake ../llvm-9.0.0.src \
      -GNinja \
      -DLLVM_BINUTILS_INCDIR=${PROJECT_ROOT_DIR}/gcc/gcc-arm-none-eabi-6-2017-q1-update/src/binutils/include \
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
      -DCMAKE_INSTALL_PREFIX=../bin \
      -DCMAKE_C_FLAGS=-fstandalone-debug \
      -DCMAKE_CXX_FLAGS=-fstandalone-debug
ninja

################################################################################