#version 460 core

uniform sampler2D MainTexture;
uniform vec4 ColorTint = vec4(1.0);
uniform float BorderThickness = 0.0;
uniform vec4 BorderColor;

uniform bool AlphaScissor = false;
uniform float AlphaScissorThreshold = 0.5;

uniform vec2 UVOffset = vec2(0.0);
uniform vec2 UVScale = vec2(1.0);

in vec2 FRAGMENT_TEXTURE_COORDS;

out vec4 FRAGMENT_COLOR;

void main() {
    vec2 uv = (FRAGMENT_TEXTURE_COORDS * UVScale) + UVOffset;
    vec4 color = texture(MainTexture, uv) * ColorTint;

    if (BorderThickness != 0.0) {
        vec2 bl = step(vec2(BorderThickness), uv);
        vec2 tr = step(vec2(BorderThickness), 1.0 - uv);
        float inside = bl.x * bl.y * tr.x * tr.y;
        color = mix(BorderColor, color, inside);
    }

    if (AlphaScissor && color.a < AlphaScissorThreshold) {
        discard;
    }

    FRAGMENT_COLOR = color;
}