#!/bin/sh

mkdir build
cd build

mkdir debug
cd debug
cmake -DCMAKE_BUILD_TYPE=Debug ../..
cd ..
mkdir release
cd release
cmake -DCMAKE_BUILD_TYPE=Release ../..
cd ../..
