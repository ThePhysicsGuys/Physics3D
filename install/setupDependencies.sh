#!/bin/sh

# gets directory of this file to find the others
INSTALLPATH="$( cd "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"
cd $INSTALLPATH/..

git clone https://github.com/microsoft/vcpkg
./vcpkg/bootstrap-vcpkg.sh

./vcpkg/vcpkg install glfw3 opengl glew freetype stb

mkdir include
cd include

git clone -b docking https://github.com/ocornut/imgui.git
cp imgui/examples/imgui_impl_opengl3.cpp imgui
cp imgui/examples/imgui_impl_opengl3.h imgui
cp imgui/examples/imgui_impl_glfw.cpp imgui
cp imgui/examples/imgui_impl_glfw.h imgui
cd ..
