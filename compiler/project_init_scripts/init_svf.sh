#!/bin/bash

set -e
set -x
COMPILER_DIR=$(readlink -f `dirname $0`/..)
PROJECT_ROOT_DIR=`dirname ${COMPILER_DIR}`
SVF_DIR=${PROJECT_ROOT_DIR}/SVF
PATCH_DIR=${COMPILER_DIR}/svf_patch

cd ${PROJECT_ROOT_DIR}
git submodule init
git submodule update
cd -

# patch -R -p0 ${SVF_DIR}/lib/MemoryModel/PointerAnalysis.cpp ${PATCH_DIR}/PointerAnalysis.patch
bash ${PATCH_DIR}/setup_symlinks.sh

cp ${COMPILER_DIR}/3rd_party/clang+llvm-10.0.0-x86_64-linux-gnu-ubuntu-18.04.tar.xz ${SVF_DIR}/llvm.tar.xz
cp ${COMPILER_DIR}/3rd_party/z3-4.8.8-x64-ubuntu-16.04.zip ${SVF_DIR}/z3.zip
cp ${COMPILER_DIR}/3rd_party/ctir-clang-v10.c3-ubuntu18.04.zip ${SVF_DIR}/ctir.zip

cd ${SVF_DIR}
./build.sh
./setup.sh

# generate patch
# diff -Naur svf_patch/PointerAnalysis.cpp svf_patch/PointerAnalysis_orig.cpp > svf_patch/PointerAnalysis.patch
