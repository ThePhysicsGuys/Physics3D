# Physics3D
A 3D physics engine, written in C++

## Project structure
The engine can be compiled separately and is found in [engine](/engine)  
There is an example application for debugging and testing included in [application](/application) and [tests](/tests)  
The graphics libraries are only necessary when building the application and the tests, so feel free to leave them be if you're only building the engine. 

## Dependencies
### Application
- [GLFW](https://www.glfw.org/) Verified working with GLFW 3.2.1  
- [GLEW](http://glew.sourceforge.net/) Verified with GLEW 2.1.0

## Setup Guide
### Visual Studio
- Clone the repository
- Create a solution in the root directory of the repository
- 'engine' on its own does not depend on any GL libraries, so if you wish to only build it then you may skip the setup for application & tests.
- Download the dependencies, and remember where you stored the .lib's and the .h's
- Import the 4 projects:
  - application
  - engine
  - tests
  - util
- Configuration:
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

## License
This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for details
