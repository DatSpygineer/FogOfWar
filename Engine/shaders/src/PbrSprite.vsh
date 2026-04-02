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

uniform uint BillboardMode;

out vec3 FRAGMENT_WORLD_POSITION;
out vec2 FRAGMENT_TEXTURE_COORDS;
out vec3 FRAGMENT_NORMAL;
out mat3 FRAGMENT_TBN;

vec3 billboard_spherical(vec3 origin, vec3 scale) {
    vec3 right = vec3(MATRIX_VIEW[0][0], MATRIX_VIEW[1][0], MATRIX_VIEW[2][0]);
    vec3 up    = vec3(MATRIX_VIEW[0][1], MATRIX_VIEW[1][1], MATRIX_VIEW[2][1]);
    return origin
    + right * VERTEX_POSITION.x * scale.x
    + up    * VERTEX_POSITION.y * scale.y;
}
vec3 billboard_cylindrical(vec3 origin, vec3 scale) {
    vec3 camera_pos = vec3(inverse(MATRIX_VIEW)[3]);
    vec3 look       = normalize(camera_pos - origin);
    look.y = 0.0;

    vec3 up    = vec3(0.0, 1.0, 0.0);
    vec3 right = cross(up, look);

    return origin
    + right * VERTEX_POSITION.x * scale.x
    + up    * VERTEX_POSITION.y * scale.y;
}

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

    vec3 position;
    if (BillboardMode == BILLBOARD_Y) {
        position = billboard_cylindrical(origin, scale);
    } else if (BillboardMode == BILLBOARD_XY) {
        position = billboard_spherical(origin, scale);
    } else {
        position = VERTEX_POSITION;
    }

    gl_Position = MATRIX_PROJECTION * MATRIX_VIEW * vec4(position, 1.0);
}