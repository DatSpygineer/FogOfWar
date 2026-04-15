#version 460 core

uniform sampler2D MainTexture;
uniform vec4 ColorTint = vec4(1.0);
uniform float BorderThickness = 0.0;
uniform vec4 BorderColor;

uniform bool AlphaScissor = false;
uniform float AlphaScissorThreshold = 0.5;

in vec2 FRAGMENT_TEXTURE_COORDS;

out vec4 FRAGMENT_COLOR;

void main() {
    vec2 bl = step(vec2(BorderThickness), FRAGMENT_TEXTURE_COORDS);
    vec2 tr = step(vec2(BorderThickness), 1.0 - FRAGMENT_TEXTURE_COORDS);
    float inside = bl.x * bl.y * tr.x * tr.y;
    vec4 bg = texture(MainTexture, FRAGMENT_TEXTURE_COORDS) * ColorTint;

    vec4 final_color = mix(BorderColor, bg, inside);

    if (AlphaScissor && final_color.a < AlphaScissorThreshold) {
        discard;
    }

    FRAGMENT_COLOR = final_color;
}