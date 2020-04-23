#!/bin/sh

sudo apt-get install libglfw3 libglfw3-dev libglew-dev git cmake

echo downloading unregistered dependencies

mkdir include
mkdir include/stb

cd include

wget -P stb https://raw.githubusercontent.com/nothings/stb/master/stb_image.h

git clone https://github.com/ocornut/imgui.git
cp imgui/examples/imgui_impl_opengl3.cpp imgui
cp imgui/examples/imgui_impl_opengl3.h imgui
cp imgui/examples/imgui_impl_glfw.cpp imgui
cp imgui/examples/imgui_impl_glfw.h imgui
cd ..