#!/bin/sh

export ARMGCC_DIR=/home/gray/embedded/gcc-arm-none-eabi/

cmake -DCMAKE_TOOLCHAIN_FILE="armgcc.cmake" -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=debug  .
make -j4
