
mkdir %~dp0..\build
mkdir %~dp0..\build_debug

cmake -DCMAKE_TOOLCHAIN_FILE=%~dp0..\vcpkg\scripts\buildsystems\vcpkg.cmake -DCMAKE_BUILD_TYPE=Release -S . -B build
cmake -DCMAKE_TOOLCHAIN_FILE=%~dp0..\vcpkg\scripts\buildsystems\vcpkg.cmake -DCMAKE_BUILD_TYPE=Debug -S . -B build_debug

