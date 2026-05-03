#version 460 core

uniform sampler2DArray MainTexture;
uniform int FrameIndex;
uniform sampler2D ColorTintMask;
uniform bool UseColorTintMask;
uniform vec4 ColorTint = vec4(1.0);
uniform bool  AlphaScissor = false;
uniform float AlphaScissorThreshold = 0.5;

in vec2 FRAGMENT_TEXTURE_COORDS;
out vec4 FRAGMENT_COLOR;

void main() {
    float tint_mask = 1.0;
    if (UseColorTintMask) {
        tint_mask = texture(ColorTintMask, FRAGMENT_TEXTURE_COORDS).r;
    }

    vec4 color_tinted = texture(MainTexture, vec3(FRAGMENT_TEXTURE_COORDS, float(FrameIndex))) * ColorTint;
    vec4 color = mix(texture(MainTexture, vec3(FRAGMENT_TEXTURE_COORDS, float(FrameIndex))), color_tinted, tint_mask);
    if (AlphaScissor && color.a < AlphaScissorThreshold) {
        discard;
    }
    FRAGMENT_COLOR = color;
}