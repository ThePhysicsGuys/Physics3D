
mkdir %~dp0..\build
mkdir %~dp0..\build\debug
mkdir %~dp0..\build\release

cmake -DCMAKE_TOOLCHAIN_FILE=%~dp0..\vcpkg\scripts\buildsystems\vcpkg.cmake -DCMAKE_BUILD_TYPE=Debug -S . -B build/debug
cmake -DCMAKE_TOOLCHAIN_FILE=%~dp0..\vcpkg\scripts\buildsystems\vcpkg.cmake -DCMAKE_BUILD_TYPE=Release -S . -B build/release

