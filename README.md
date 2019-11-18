# Physics3D
A 3D physics engine, written in C++

![Cool Shapes](https://i.imgur.com/neeHoJh.png)
![Many Shapes](https://i.imgur.com/h62AI58.jpg)

## Project structure
The engine can be compiled separately and is found in [engine](/engine)  
There is an example application for debugging and testing included in [application](/application) and [tests](/tests)  
The graphics libraries are only necessary when building the application and the tests, so feel free to leave them be if you're only building the engine. 

## Dependencies
### Application & graphics
- [GLFW](https://www.glfw.org/) Verified working with GLFW 3.2.1  
- [GLEW](http://glew.sourceforge.net/) Verified with GLEW 2.1.0
- [stb_image](https://github.com/nothings/stb) Verified with stb_image.h v2.22
- [FreeType](https://www.freetype.org/) Verified with FreeType v2.9.0

## Setup Guide
### Visual Studio
- Clone the repository
- 'physics' on its own does not depend on any GL libraries, so if you wish to only build it then you may skip the setup for application & tests.
- Download the dependencies, the Visual Studio configuration expects the libraries to be stored in Physics3D/lib/
  includes should be stored in Physics3D/include/
  With Physics3D/ the root folder of the git project. 
  Your project structure should look like this:  
  Physics3D/  
  &nbsp;&nbsp;&nbsp;&nbsp;    include/  
  &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;      freetype/  
  &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;      GL/  
  &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;      GLFW/  
  &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;      stb/  
  &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;      ft2build.h  
  &nbsp;&nbsp;&nbsp;&nbsp;    lib/  
  &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;      freetype.lib  
  &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;      glew32.lib  
  &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;      glew32s.lib  
  &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;      glfw3.dll  
  &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;      glfw3.lib  
  &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;      glfw3dll.lib  
  &nbsp;&nbsp;&nbsp;&nbsp;    (Project and other files)  
- Configuration (Should already be configured in the provided project and solution files):
  - General > Configuration type: 
    - application & tests: Application (.exe)
    - engine & util: Static library (.lib)
  - C/C++ > General > Additional include directories:
    - application & tests: "$(SolutionDir)include" (or wherever you stored the headers)
  - C/C++ > Preprocessor > Preprocessor Definitions:
    - application & tests: Add "GLEW_STATIC; " to the front of the list, you should be left with something like "GLEW_STATIC ; _MBCS;%(PreprocessorDefinitions)"
  - Linker > General > Additional Library Directories:
    - application & tests: "$(SolutionDir)lib;$(OutDir)" (lib can be whatever path you wish, but $(OutDir) is very important as it contains the .lib's of the other projects being built!
  - Linker > Input > Additional Dependencies:
    - application & tests: "kernel32.lib;user32.lib;gdi32.lib;shell32.lib;glfw3.lib;glew32s.lib;opengl32.lib;engine.lib;util.lib"
- You are done!

## Authors
* **Lennart Van Hirtum** - [VonTum](https://github.com/VonTum)
* **Matthias Vandersanden** - [olympus112](https://github.com/olympus112)

This list is inconclusive as new contributors are always welcome! 

## License
This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for details
