#!/usr/bin/env bash

ARCH=$1
SRC_PATH=$2
OUTPUT_PATH=$3

if [ -z ${ARCH} ] || [ -z ${SRC_PATH} ] || [ -z ${OUTPUT_PATH} ]; then
    echo "usage: compile.sh <ARCH> <SRC_PATH> <OUTPUT_PATH>"
    echo ""
    echo "options:"
    echo "  ARCH           the target architecture for cross-compilation (x86_64|i686|armv7l|aarch64)"
    echo "  SRC_PATH       the absolute path to the source folder"
    echo "  OUTPUT_PATH    the absolute path to the output folder"
    exit 1
fi

OUTPUT_PATH=${OUTPUT_PATH}/${ARCH}

if [ ${ARCH} = "x86_64" ]; then
    TOOLCHAIN_FILE=''
else
    TOOLCHAIN_FILE=${SRC_PATH}/cmake/gcc-${ARCH}.cmake
fi

cmake -G 'Unix Makefiles' -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=${TOOLCHAIN_FILE} ${SRC_PATH}
make
mkdir -p ${OUTPUT_PATH}
mv bable-bridge-linux ${OUTPUT_PATH}
