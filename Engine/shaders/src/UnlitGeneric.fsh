#version 460 core
#extension GL_ARB_shading_language_include : require

uniform sampler2D MainTexture;
uniform vec4  ColorTint = vec4(1.0);
uniform bool  AlphaScissor = false;
uniform float AlphaScissorThreshold = 0.5;

#include "/fragment_shared.glsl"

in vec3 FRAGMENT_WORLD_POSITION;
in vec2 FRAGMENT_TEXTURE_COORDS;
in vec3 FRAGMENT_NORMAL;
in mat3 FRAGMENT_TBN;

out vec4 FRAGMENT_COLOR;

void main() {
    vec4 tex = texture(MainTexture, FRAGMENT_TEXTURE_COORDS);
    if (AlphaScissor && tex.a < AlphaScissorThreshold) {
        discard;
    } else {
        FRAGMENT_COLOR = tex * ColorTint;
    }
}