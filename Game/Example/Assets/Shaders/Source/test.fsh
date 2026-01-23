#version 330 core

in vec3 FRAGMENT_VERTEX_POSITION;
in vec3 FRAGMENT_NORMAL;
in vec2 FRAGMENT_TEXTURE_COORDS;

uniform sampler2D MainTexture;
uniform vec4 TextureColor;

out vec4 FRAGMENT_COLOR;

void main() {
    FRAGMENT_COLOR = texture(MainTexture, FRAGMENT_TEXTURE_COORDS) * TextureColor;
}