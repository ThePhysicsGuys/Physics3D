#!/bin/sh

sudo add-apt-repository universe
sudo apt-get install build-essential
sudo apt-get install freeglut3
sudo apt-get install freeglut3-dev
sudo apt-get install mesa-common-dev
sudo apt-get install libglfw3
sudo apt-get install libglfw3-dev
sudo apt-get install libglew-dev
sudo apt-get install git
sudo apt-get install libfreetype6-dev
sudo apt install cmake

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
