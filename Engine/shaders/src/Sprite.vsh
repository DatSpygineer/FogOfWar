#version 460 core

#define MAX_INSTANCE_COUNT 100

#define BILLBOARD_Y  1
#define BILLBOARD_XY 2

layout (location = 0) in vec3 VERTEX_POSITION;
layout (location = 1) in vec3 VERTEX_NORMAL;
layout (location = 2) in vec3 VERTEX_TANGENT;
layout (location = 3) in vec3 VERTEX_BITANGENT;
layout (location = 4) in vec2 VERTEX_TEXTURE_COORDS;

uniform mat4 MATRIX_PROJECTION;
uniform mat4 MATRIX_VIEW;
uniform mat4 MATRIX_MODEL[MAX_INSTANCE_COUNT];

uniform uint BillboardMode;

out vec2 FRAGMENT_TEXTURE_COORDS;

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
    FRAGMENT_TEXTURE_COORDS = VERTEX_TEXTURE_COORDS;

    vec3 origin = vec3(model[3][0], model[3][1], model[3][2]);
    vec3 scale = vec3(
        length(vec3(model[0][0], model[0][1], model[0][2])),
        length(vec3(model[1][0], model[1][1], model[1][2])),
        length(vec3(model[2][0], model[2][1], model[2][2]))
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