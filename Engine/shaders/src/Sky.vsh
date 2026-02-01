#version 460 core

layout (location = 0) in vec3 VERTEX_POSITION;

uniform mat4 MATRIX_PROJECTION;
uniform mat4 MATRIX_VIEW;

out vec3 FRAGMENT_SKY_COORDS;

void main() {
    FRAGMENT_SKY_COORDS = VERTEX_POSITION;
    gl_Position = MATRIX_PROJECTION * MATRIX_VIEW * vec4(VERTEX_POSITION, 1.0);
}