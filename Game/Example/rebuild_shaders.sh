#!/bin/bash

rm -rf shaderlib

cd ../../Build/cmake-Debug
cmake ../..
rm ../Games/FogOfWarGameExample/libshaderlib.so
ninja FogOfWarGameExample_ShaderLib