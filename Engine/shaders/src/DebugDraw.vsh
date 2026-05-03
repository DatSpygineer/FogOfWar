#version 330 core

layout (location = 0) in vec3 VERTEX_POSITION;
layout (location = 1) in vec4 VERTEX_COLOR;

uniform mat4 MATRIX_PROJECTION;
uniform mat4 MATRIX_VIEW;
uniform mat4 MATRIX_MODEL;

uniform float POINT_SIZE = 1.0;

out vec4 FRAGMENT_VERTEX_COLOR;

void main() {
    FRAGMENT_VERTEX_COLOR = VERTEX_COLOR;
    gl_PointSize = POINT_SIZE;
    gl_Position = MATRIX_PROJECTION * MATRIX_VIEW * MATRIX_MODEL * vec4(VERTEX_POSITION, 1.0);
}