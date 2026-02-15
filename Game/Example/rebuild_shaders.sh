#!/bin/bash

rm -rf shaderlib

cd ../../Build/cmake-Debug
cmake ../..
rm ../Games/FogOfWarGameExample/libshaderlib.so

if command -v "ninja" >/dev/null 2>&1; then
  ninja FogOfWarGameExample_ShaderLib
else
  make FogOfWarGameExample_ShaderLib
fi