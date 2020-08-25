#!/bin/sh

cd ..

mkdir build
mkdir build\debug
mkdir build\release

cmake -DCMAKE_BUILD_TYPE=Debug -S . -B build/debug
cmake -DCMAKE_BUILD_TYPE=Release -S . -B build/release
