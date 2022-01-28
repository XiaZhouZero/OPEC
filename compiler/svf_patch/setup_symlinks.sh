#!/bin/bash

set -e
set -x
REPO_ROOT=$(readlink -f `dirname $0`/../..)
PROJECT_ROOT_DIR=${REPO_ROOT}

SVF_DIR=${PROJECT_ROOT_DIR}/SVF

SYM_LINK='ln -sfn'
echo $PROJECT_ROOT_DIR
echo $SVF_DIR

# soft link for json libs
${SYM_LINK} ${PROJECT_ROOT_DIR}/compiler/svf_patch/json-include ${SVF_DIR}/include/Util/json
${SYM_LINK} ${PROJECT_ROOT_DIR}/compiler/svf_patch/jsoncpp.cpp ${SVF_DIR}/lib/Util/jsoncpp.cpp

# soft link for SVF files
${SYM_LINK} ${PROJECT_ROOT_DIR}/compiler/svf_patch/PointerAnalysis.h ${SVF_DIR}/include/MemoryModel/PointerAnalysis.h
${SYM_LINK} ${PROJECT_ROOT_DIR}/compiler/svf_patch/WPAStat.h ${SVF_DIR}/include/WPA/WPAStat.h
${SYM_LINK} ${PROJECT_ROOT_DIR}/compiler/svf_patch/PointerAnalysis.cpp ${SVF_DIR}/lib/MemoryModel/PointerAnalysis.cpp
${SYM_LINK} ${PROJECT_ROOT_DIR}/compiler/svf_patch/AndersenStat.cpp ${SVF_DIR}/lib/WPA/AndersenStat.cpp

${SYM_LINK} ${PROJECT_ROOT_DIR}/compiler/svf_patch/build.sh ${SVF_DIR}/build.sh