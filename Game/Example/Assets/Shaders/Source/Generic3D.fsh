#version 330 core

#define MAX_LIGHTS 32

struct LightInfo {
    vec3 Position;
    vec3 Color;
};

uniform sampler2D MainTexture;
uniform sampler2D NormalMap;
uniform sampler2D SpecularMap;
uniform float AmbientLightStrength = 0.25;
uniform vec3  ViewPos;
uniform float PhongStrength = 1.0;
uniform float PhongExponent = 1.0;

uniform LightInfo Lights[MAX_LIGHTS];
uniform int LightCount;

in vec3 FRAGMENT_WORLD_POSITION;
in vec2 FRAGMENT_TEXTURE_COORDS;
in vec3 FRAGMENT_NORMAL;
in vec3 FRAGMENT_WORLD_NORMAL;

out vec4 FRAGMENT_COLOR;

vec3 light_diffuse(vec3 light_dir, vec3 color) {
    return vec3(dot(normalize(FRAGMENT_WORLD_NORMAL), light_dir) * color);
}
vec3 light_specular(vec3 view_dir, vec3 light_dir, vec3 color, float phong_strength, float phong_exponent) {
    vec3 halfway_dir = normalize(light_dir + view_dir);
    float spec = pow(max(dot(normalize(FRAGMENT_WORLD_NORMAL), halfway_dir), 0.0), phong_exponent);
    return phong_strength * spec * color;
}

void main() {
    vec4 albedo      = texture(MainTexture, FRAGMENT_TEXTURE_COORDS);
    vec2 spec_map    = texture(SpecularMap, FRAGMENT_TEXTURE_COORDS).xy * vec2(PhongStrength, PhongExponent);
    vec3 light       = vec3(1.0);

    for (int i = 0; i  < LightCount; i++) {
        vec3 view_dir    = normalize(ViewPos - FRAGMENT_WORLD_POSITION);
        vec3 light_dir   = normalize(Lights[i].Position - FRAGMENT_WORLD_POSITION);
        vec3 light_color = Lights[i].Color;
        light += light_diffuse(light_dir, light_color) + light_specular(view_dir, light_dir, Lights[i].Color, spec_map.x, spec_map.y);
    }

    FRAGMENT_COLOR = vec4(light, 1.0) * albedo;
}