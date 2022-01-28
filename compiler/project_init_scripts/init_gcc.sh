#!/bin/bash
#cd COMPILER_DIR
#./project_init_scripts/init_gcc.sh

set -e
set -x
COMPILER_DIR=$(readlink -f `dirname $0`/..)
PROJECT_ROOT_DIR=`dirname ${COMPILER_DIR}`
YEAR=`date +%Y`
QUATER=`date +%q`
PKG_NAME=gcc-arm-none-eabi-6-${YEAR}-q${QUATER}-update   # Check the version once you rebuild

######################     Build GCC    ########################################
# Checks to see if the appropriate version of GCC has been build and placed at
# the correct location, if not builds it, using the archive src in this
# repo.  Uses a slightly modified (Builds linker with plugin support) archive.
################################################################################
if [ ! -e ${PROJECT_ROOT_DIR}/gcc/bins ]
then
  echo "gcc-arm-none-eabi-6-2017-q1-update with gold plugin will be builded"
  echo "preparing..."
  mkdir -p ${PROJECT_ROOT_DIR}/gcc
  cd ${PROJECT_ROOT_DIR}/gcc
  if [ ! -e gcc-arm-none-eabi-6-2017-q1-update/pkg ]
  then

     cat ${COMPILER_DIR}/3rd_party/gcc-arm-none-eabi-6-2017-q1-update.tar.bz2* >./gcc-arm-none-eabi-6-2017-q1-update.tar.bz2
     tar -xjf gcc-arm-none-eabi-6-2017-q1-update.tar.bz2
     cd gcc-arm-none-eabi-6-2017-q1-update
     cp ${COMPILER_DIR}/project_init_scripts/gcc_build_toolchain.sh build-toolchain.sh
     cd src
     find -name '*.tar.*' | xargs -I% tar -xf %
     cd ..
     ./build-prerequisites.sh --skip_steps=mingw32
     ./build-toolchain.sh --skip_steps=mingw32
     cd ${PROJECT_ROOT_DIR}/gcc
  fi
  cd ${PROJECT_ROOT_DIR}/gcc/gcc-arm-none-eabi-6-2017-q1-update/pkg
  tar -xjf ${PKG_NAME}-linux.tar.bz2
  mv ${PKG_NAME} ../../bins
  cd ${COMPILER_DIR}
fi
###############################################################################
