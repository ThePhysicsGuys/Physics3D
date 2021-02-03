#!/bin/sh

# gets directory of this file to find the others
INSTALLPATH="$( cd "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"
cd $INSTALLPATH/..

mkdir build
mkdir build_debug

toolpath="$PWD/vcpkg/scripts/buildsystems/vcpkg.cmake"

cmake -DCMAKE_TOOLCHAIN_FILE=$toolpath -DCMAKE_BUILD_TYPE=Release -S . -B build
cmake -DCMAKE_TOOLCHAIN_FILE=$toolpath -DCMAKE_BUILD_TYPE=Debug -S . -B build_debug

