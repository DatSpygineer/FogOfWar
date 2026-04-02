#version 460 core

#define MAX_INSTANCE_COUNT 100

layout (location = 0) in vec3 VERTEX_POSITION;
layout (location = 1) in vec3 VERTEX_NORMAL;
layout (location = 2) in vec3 VERTEX_TANGENT;
layout (location = 3) in vec3 VERTEX_BITANGENT;
layout (location = 4) in vec2 VERTEX_TEXTURE_COORDS;

uniform mat4 MATRIX_PROJECTION;
uniform mat4 MATRIX_VIEW;
uniform mat4 MATRIX_MODEL[MAX_INSTANCE_COUNT];

out vec3 FRAGMENT_WORLD_POSITION;
out vec2 FRAGMENT_TEXTURE_COORDS;
out vec3 FRAGMENT_NORMAL;
out mat3 FRAGMENT_TBN;

void main() {
    mat4 model = MATRIX_MODEL[gl_InstanceID];
    FRAGMENT_WORLD_POSITION = vec3(model * vec4(VERTEX_POSITION, 1.0));
    FRAGMENT_TEXTURE_COORDS = VERTEX_TEXTURE_COORDS;
    FRAGMENT_NORMAL         = VERTEX_NORMAL;
    FRAGMENT_TBN = mat3(
        normalize(vec3(model * vec4(VERTEX_TANGENT,   0.0))),
        normalize(vec3(model * vec4(VERTEX_BITANGENT, 0.0))),
        normalize(vec3(model * vec4(VERTEX_NORMAL,    0.0)))
    );

    gl_Position = MATRIX_PROJECTION * MATRIX_VIEW * model * vec4(VERTEX_POSITION, 1.0);
}