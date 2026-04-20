#version 460 core

uniform sampler2D MainTexture;
uniform vec4 ColorTint = vec4(1.0);
uniform float BorderThickness;
uniform vec4 BorderColor;
uniform float Radius;

uniform bool AlphaScissor = false;
uniform float AlphaScissorThreshold = 0.5;

in vec2 FRAGMENT_TEXTURE_COORDS;
in vec2 FRAGMENT_RECT_SIZE;

out vec4 FRAGMENT_COLOR;

float sdf_rounded_box(vec2 p, vec2 b, float r) {
    vec2 q = abs(p) - b + r;
    return min(max(q.x, q.y), 0.0) + length(max(q, 0.0)) - r;
}

void main() {
    vec2 half_size = FRAGMENT_RECT_SIZE * 0.5;
    vec2 p = (FRAGMENT_TEXTURE_COORDS - 0.5) * FRAGMENT_RECT_SIZE;

    float dist = sdf_rounded_box(p, half_size, Radius);
    float edge_softness = fwidth(dist);

    float outer_alpha = 1.0 - smoothstep(-edge_softness, edge_softness, dist);

    float inner_alpha = 1.0 - smoothstep(-edge_softness, edge_softness, dist + BorderThickness);

    vec4 bg = texture(MainTexture, FRAGMENT_TEXTURE_COORDS) * ColorTint;

    vec4 res_color = mix(BorderColor, bg, inner_alpha);
    res_color.a *= outer_alpha;

    if ((AlphaScissor && res_color.a < AlphaScissorThreshold) || res_color.a < 0.01) {
        discard;
    }

    FRAGMENT_COLOR = res_color;
}