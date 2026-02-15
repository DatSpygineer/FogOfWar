@echo off

rd /s/q shaderlib

cd ../../Build/cmake-Debug
cmake ../..
rd /s/q ../Games/FogOfWarGameExample/shaderlib.*
ninja FogOfWarGameExample_ShaderLib