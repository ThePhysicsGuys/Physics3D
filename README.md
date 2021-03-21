# Physics3D
![Ubuntu CI Badge](https://github.com/ThePhysicsGuys/Physics3D/workflows/Ubuntu/badge.svg)
![MSVC CI Badge](https://github.com/ThePhysicsGuys/Physics3D/workflows/MSVC/badge.svg)

A 3D physics engine, written in C++

![Many Shapes](https://media.discordapp.net/attachments/378983421936140300/638494259452051480/unknown.png?width=1152&height=664)
![Different Shapes](https://i.imgur.com/h62AI58.jpg)
![Editor](https://cdn.discordapp.com/attachments/378983421936140300/779017688650743828/Screenshot_1.png)

## Project structure
The Physics3D project consists of 7 projects, each with its own role:
- The [physics](/physics) project contains the physics engine can be compiled separately.  
- The [graphics](/graphics) project contains all the logic to interact with OpenGL, visual debugging and gui code.
- The [engine](/engine) project contains general concepts that can be applied in multiple environments like a layer systems and an event system.
- The [util](/util) project contains code that can be used in all project, like logging, utilities and resource management.
- The [application](/application) project contains an executable example application for visualizing, debugging and testing the physics engine. This project depends on the engine, graphics and physics project. Every project, including the physics project depends on util. 
- The [tests](/tests) project contains an executable with unit test for the physics engine.
- The [benchmarks](/benchmarks) project contains an executable with benchmarks to evaluate the physics engine's performance.

## Dependencies
### Application & engine & graphics
- [GLFW](https://www.glfw.org/) Verified working with GLFW 3.2.1  
- [GLEW](http://glew.sourceforge.net/) Verified with GLEW 2.1.0
- [stb_image](https://github.com/nothings/stb) Verified with stb_image.h v2.22
- [FreeType](https://www.freetype.org/) Verified with FreeType v2.9.0
- [Dear ImGui](https://github.com/ocornut/imgui/tree/docking) Verified with ImGui v1.77. Make sure to grab the files from the experimental docking branch.
- some OpenGL functionality may not be available on virtual machines, enabling 3D acceleration might solve this

## Setup Guide

### CMake
If you suddenly can't build the project anymore after pulling, perhaps one of the dependencies has changed. Try running [install/clean.sh](/install/clean.sh) (Unix) or [install/clean.bat](/install/clean.bat) (Windows) and rerun the setup script in the steps below. 

If you still have build problems, please create an issue as we want setting up to be as easy as possible. 

#### Platform independent using vcpkg
1. Clone the repository
2. If you do not have cmake already: download it from [cmake.org/download/](https://cmake.org/download/)
3. Run [install/setup.sh](/install/setup.sh) (Unix) or [install/setup.bat](/install/setup.bat) (Windows) from the Physics3D directory, this will install the necessary dependencies using [vcpkg](https://github.com/microsoft/vcpkg) and create the build folders. It will also run cmake for debug and release. 
  The dependencies can be installed on their own with [install/setupDependencies.sh](/install/setupDependencies.sh) (Unix) or [install/setupDependencies.bat](/install/setupDependencies.bat) (Windows)
  The build directories can be generated on their own with [install/setupBuild.sh](/install/setupBuild.sh) (Unix) or [install/setupBuild.bat](/install/setupBuild.bat) (Windows)
4. Make the build from the build directory `cd build` with `cmake --build .`
5. To run the application, you must also run it from the build directory: `cd build` `./application`. Tests and benchmarks can be run from anywhere. 

#### Ubuntu specific using apt-get
If you are using Ubuntu, we recommend using this installation method instead, as setting up using vcpkg can take a very long time. This method should get you a working version of the engine starting from a clean Ubuntu 18.04. 

1. Clone the repository
2. Run [install/setupUbuntu.sh](/install/setupUbuntu.sh) from the Physics3D directory, this will install the necessary dependencies and create the build folders. It will also run cmake for debug and release. 
  The dependencies can be installed on their own with [install/setupDependenciesUbuntu.sh](/install/setupDependenciesUbuntu.sh)
  The build directories can be generated on their own with [install/setupBuildUbuntu.sh](/install/setupBuildUbuntu.sh)
3. Make the build from the build directory `cd build` with `cmake --build .`
4. To run the application, you must also run it from the build directory: `./application`. Tests and benchmarks can be run from anywhere. 

### Visual Studio
1. Clone the repository
2. The physics project on its own does not depend on any libraries, so if you wish to only build it then you may skip step 3.
3. Download the dependencies, the Visual Studio configuration expects the libraries to be stored in Physics3D/lib/, includes should be stored in Physics3D/include/, with Physics3D/ the root folder of the git project. 
  Your project structure should look like this:  
  Physics3D/  
  | - include/  
  | | - freetype/  
  | | | - (freetype headers)  
  | | - GL  
  | | | - (glew headers)  
  | | - GLFW/  
  | | | - (glfw3 headers)  
  | | - imgui/  
  | | | - imconfig.h  
  | | | - imgui.h & .cpp  
  | | | - imgui_draw.h  
  | | | - imgui_internal.h  
  | | | - imgui_widgets.cpp  
  | | | - imgui_rectpack.h  
  | | | - imgui_textedit.h  
  | | | - imgui_truetype.h  
  | | | - imgui_impl_glfw.h & .cpp  
  | | | - imgui_impl_opengl3.h & .cpp  
  | | - stb_image.h  
  | | - ft2build.h  
  | - lib/  
  | | - freetype.lib  
  | | - glew32.lib  
  | | - glew32s.lib  
  | | - glfw3.lib   
  | - (Project and other files)  
4. The configuration should already be configured in the provided project and solution files
5. You are done!

## Authors
* **Lennart Van Hirtum** - [VonTum](https://github.com/VonTum)
* **Matthias Vandersanden** - [olympus112](https://github.com/olympus112)

This list is inconclusive as new contributors are always welcome! 

## License
This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for details
