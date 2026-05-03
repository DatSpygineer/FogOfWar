#version 460 core

uniform sampler2DArray MainTexture;
uniform vec2 UVStart = vec2(0.0);
uniform vec2 UVEnd   = vec2(1.0);

uniform bool  AlphaScissor = false;
uniform float AlphaScissorThreshold = 0.5;

in vec2 FRAGMENT_TEXTURE_COORDS;
out vec4 FRAGMENT_COLOR;

void main() {
    vec2 uv = mix(UVStart, UVEnd, fract(FRAGMENT_TEXTURE_COORDS));

    vec4 color = texture(MainTexture, uv);

    if (AlphaScissor && color.a < AlphaScissorThreshold) {
        discard;
    }

    FRAGMENT_COLOR = color;
}