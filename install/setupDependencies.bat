
cd %~dp0..

if NOT EXIST vcpkg (
	git clone https://github.com/microsoft/vcpkg

	call .\vcpkg\bootstrap-vcpkg.bat
)

.\vcpkg\vcpkg.exe install glfw3 opengl glew freetype stb --triplet x64-windows

mkdir include

cd include

git clone -b docking https://github.com/ocornut/imgui.git
copy imgui\examples\imgui_impl_opengl3.cpp imgui
copy imgui\examples\imgui_impl_opengl3.h imgui
copy imgui\examples\imgui_impl_glfw.cpp imgui
copy imgui\examples\imgui_impl_glfw.h imgui

cd ..

