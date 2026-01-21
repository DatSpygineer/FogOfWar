#version 330 core

in vec3 FRAGMENT_VERTEX_POSITION;
in vec3 FRAGMENT_NORMAL;
in vec2 FRAGMENT_TEXTURE_COORDS;

uniform sampler2D MainTexture;

out vec4 FRAGMENT_COLOR;

void main() {
    vec4 tex = texture(MainTexture, FRAGMENT_TEXTURE_COORDS);
    FRAGMENT_COLOR = vec4(tex.rgb * (((FRAGMENT_NORMAL.x + FRAGMENT_NORMAL.y + FRAGMENT_NORMAL.z) / 3.0) * 0.5 + 0.5), 1.0);
}