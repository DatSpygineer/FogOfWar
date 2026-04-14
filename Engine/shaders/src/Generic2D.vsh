#version 460 core

layout (location = 0) in vec2 VERTEX_POSITION;
layout (location = 1) in vec2 VERTEX_TEXTURE_COORDS;

uniform int DEPTH = 0;
uniform mat4 MATRIX_PROJECTION;
uniform vec2 AreaPosition;
uniform vec2 AreaSize;

out vec2 FRAGMENT_TEXTURE_COORDS;

void main() {
    FRAGMENT_TEXTURE_COORDS = VERTEX_TEXTURE_COORDS;
    gl_Position = MATRIX_PROJECTION * vec4(VERTEX_POSITION.xy * AreaSize + AreaPosition, float(DEPTH), 0.0);
}