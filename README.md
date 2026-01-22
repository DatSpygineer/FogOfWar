# Fog Of War Engine
A game engine written in C++23, using OpenGL 4.6 core.

## How to build
### Windows
*TODO*
### Linux
To build the project, you need CMake 3.30 or newer.<br>
Before compile, make sure you have the following libraries installed:
- libzip
- glm
- glfw
- assimp
- freetype2

Install dependencies on Ubuntu/Debian:
```shell
sudo apt install libzip-dev libglm-dev libglfw3-dev libfreetype-dev -y
```
Execute the shell script `build.sh`. You can use `--debug` argument to build the debug version.
