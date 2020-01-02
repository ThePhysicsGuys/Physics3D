# Physics3D
A 3D physics engine, written in C++

![Many Shapes](https://media.discordapp.net/attachments/378983421936140300/638494259452051480/unknown.png?width=1152&height=664)
![Different Shapes](https://i.imgur.com/h62AI58.jpg)
![Editor](https://media.discordapp.net/attachments/378983421936140300/662330290894798879/unknown.png?width=1239&height=664)

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
- [ImGui](https://github.com/ocornut/imgui) Verified with ImGui v1.74

## Setup Guide
### Visual Studio
1. Clone the repository
2. The physics project on its own does not depend on any libraries, so if you wish to only build it then you may skip step 3.
3. Download the dependencies, the Visual Studio configuration expects the libraries to be stored in Physics3D/lib/, includes should be stored in Physics3D/include/, with Physics3D/ the root folder of the git project. 
  Your project structure should look like this:  
  Physics3D/  
  | - include/  
  | | - freetype/ (freetype headers)
  | | - GL/ (glew headers)
  | | - GLFW/ (glfw3 headers)
  | | - imgui/ (imgui headers)
  | | - stb/ (stb_image.h)
  | | - ft2build.h
  | - lib/  
  | | - freetype.lib  
  | | - glew32.lib  
  | | - glew32s.lib  
  | | - glfw3.dll  
  | | - glfw3.lib  
  | | - glfw3dll.lib  
  | - (Project and other files)  
4. The configuration should already be configured in the provided project and solution files
5. You are done!

## Authors
* **Lennart Van Hirtum** - [VonTum](https://github.com/VonTum)
* **Matthias Vandersanden** - [olympus112](https://github.com/olympus112)

This list is inconclusive as new contributors are always welcome! 

## License
This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for details
