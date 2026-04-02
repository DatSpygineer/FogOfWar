#version 460 core

uniform sampler2D MainTexture;
uniform vec4 ColorTint = vec4(1.0);
uniform bool  AlphaScissor = false;
uniform float AlphaScissorThreshold = 0.5;

in vec2 FRAGMENT_TEXTURE_COORDS;
out vec4 FRAGMENT_COLOR;

void main() {
    vec4 color = texture(MainTexture, FRAGMENT_TEXTURE_COORDS) * ColorTint;
    if (AlphaScissor && color.a < AlphaScissorThreshold) {
        discard;
    }
    FRAGMENT_COLOR = color;
}