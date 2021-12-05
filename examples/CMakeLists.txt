cmake_minimum_required(VERSION 3.10)

project(Physics3D-OpenGL-Demo VERSION 1.0)

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Building with: ${CMAKE_CXX_COMPILER_ID}")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /arch:AVX2")
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /O2")
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -march=native")
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
endif()

add_executable(openglBasic 
	openglBasic.cpp
)

# OpenGL, GLEW, GLFW
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 REQUIRED)

target_link_libraries(openglBasic glfw)
target_link_libraries(openglBasic OpenGL::GL)
target_link_libraries(openglBasic GLEW::GLEW)

# Physics3D
add_subdirectory(../Physics3D build)

target_include_directories(openglBasic PUBLIC ../)
target_link_libraries(openglBasic Physics3D)
