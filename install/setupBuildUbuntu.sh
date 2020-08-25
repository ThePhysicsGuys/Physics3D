#!/bin/sh

# gets directory of this file to find the others
INSTALLPATH="$( cd "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"
cd $INSTALLPATH/..

mkdir build
mkdir build/debug
mkdir build/release

cmake -DCMAKE_BUILD_TYPE=Debug -S . -B build/debug
cmake -DCMAKE_BUILD_TYPE=Release -S . -B build/release
