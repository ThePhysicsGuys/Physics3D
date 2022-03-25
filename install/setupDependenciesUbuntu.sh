#!/bin/sh

INSTALLPATH="$( cd "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"
cd $INSTALLPATH/..

sudo add-apt-repository universe
sudo apt update
sudo apt-get install build-essential freeglut3 freeglut3-dev mesa-common-dev libglfw3 libglfw3-dev libglew-dev git libfreetype6-dev
sudo apt install cmake


mkdir include
cd include

wget https://raw.githubusercontent.com/nothings/stb/master/stb_image.h

git clone -b docking https://github.com/ocornut/imgui.git
cd imgui
git checkout 05bc204dbd80dfebb3dab1511caf1cb980620c76
cd ..
cp imgui/examples/imgui_impl_opengl3.cpp imgui
cp imgui/examples/imgui_impl_opengl3.h imgui
cp imgui/examples/imgui_impl_glfw.cpp imgui
cp imgui/examples/imgui_impl_glfw.h imgui
cd ..
