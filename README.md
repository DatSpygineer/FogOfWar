# Fog Of War Engine
A game engine written in C++23, with support for C++20, using OpenGL 4.6 core profile.

## Project sturcture
This project is made out of 3 subprojects:
- Shared (Code shared by the 2 other subproject.)
- Renderer
- Engine

## How to build
To build this project you'll need CMake 3.25 or newer and a C++ compiler that supports C++ 23.<br>
If you are using Windows, install [VCPKG](https://github.com/microsoft/vcpkg) as well.

Recommended minimal compiler versions:
- gcc 14
- clang 18

### Dependencies
Before compile, make sure you have the following libraries installed:
- libzip
- glfw
- assimp
- freetype2
- google test

#### Install dependencies on Ubuntu/Debian:
```shell
sudo apt install libzip-dev libglfw3-dev libassimp-dev libfreetype-dev googletest -y
```
#### Install dependencies on Fedora:
```shell
sudo dnf install libzip glfw assimp freetype gtest
```
#### Install dependencies on Arch:
```shell
# TODO
```
#### Install dependencies via VCPKG:
Open terminal in the project root and run:
```shell
vcpkg install
```

On Linux, execute the shell script `build.sh`, on Windows use `build.bat`. You can use `--debug` argument to build the debug version.

**NOTE**: Windows is not supported at the moment.
