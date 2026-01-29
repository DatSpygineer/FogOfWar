# Fog Of War Engine
A game engine written in C++23, using OpenGL 4.6 core profile.

## Project sturcture
This project is made out of 3 subprojects:
- Shared (Code shared by the 2 other subproject.)
- Renderer
- Engine

## How to build
To build this project you'll need CMake 3.25 or newer and a C++ compiler that supports C++ 23.

Recommended minimal compiler versions:
- gcc 14
- clang 18

## Dependencies:
- Linux:
    - X11 or wayland development libraries
- All platforms:
    - OpenGL 4.6

On Linux, execute the shell script `build.sh`, on Windows use `build.bat`. You can use `--debug` argument to build the debug version.

**NOTE**: The project doesn't compile with MSVC at the moment!
