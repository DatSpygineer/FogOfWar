#version 460 core

uniform samplerCube SkyTexture;
in vec3 FRAGMENT_SKY_COORDS;
out vec4 FRAGMENT_COLOR;

void main() {
    FRAGMENT_COLOR = texture(SkyTexture, FRAGMENT_SKY_COORDS);
}