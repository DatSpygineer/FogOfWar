#version 460 core
#extension GL_ARB_shading_language_include : require

#define MAX_LIGHTS 32
#include "/phong_shared.glsl"

uniform sampler2D MainTexture;
uniform sampler2D NormalMap;
uniform sampler2D SpecularMap;
uniform sampler2D SelfIllumMask;
uniform samplerCube EnvMap;
uniform vec3  ViewPos;
uniform bool  AlphaScissor = false;
uniform float AlphaScissorThreshold = 0.5;

#include "/fragment_shared.glsl"

in vec3 FRAGMENT_WORLD_POSITION;
in vec2 FRAGMENT_TEXTURE_COORDS;
in vec3 FRAGMENT_NORMAL;
in mat3 FRAGMENT_TBN;

out vec4 FRAGMENT_COLOR;

void main() {
    vec4 albedo     = texture(MainTexture, FRAGMENT_TEXTURE_COORDS);
    vec3 specmap    = texture(SpecularMap, FRAGMENT_TEXTURE_COORDS).rgb;
    vec3 normal_map = texture(NormalMap, FRAGMENT_TEXTURE_COORDS).xyz;
    float selfillum = texture(SelfIllumMask, FRAGMENT_TEXTURE_COORDS).r;

    vec3 norm     = normalize(FRAGMENT_TBN * (normal_map * 2.0 - 1.0));
    vec3 view_dir = normalize(ViewPos - FRAGMENT_WORLD_POSITION);

    PhongParams params;
    params.Albedo = albedo;
    params.PhongStrength  = PhongStrength * specmap.x;
    params.PhongExponent  = clamp(PhongExponent * specmap.y, 0.01, 1.0) * 256;
    params.EnvMapStrength = EnvMapStrength * specmap.z;
    params.Albedo += vec4(envmap_refl(EnvMap, params, norm, ViewPos, FRAGMENT_WORLD_POSITION), 1.0);

    vec4 color = light_directional(Environment, params, norm, view_dir);
    for (int i = 0; i < LightCount; ++i) {
        color += light_point(Lights[i], Environment, params, norm, FRAGMENT_WORLD_POSITION, view_dir);
    }

    float alpha = albedo.a;
    if (AlphaScissor && alpha < AlphaScissorThreshold) {
        discard;
    } else {
        FRAGMENT_COLOR = vec4(mix(color.rgb, color.rgb + albedo.rgb, selfillum), alpha);
    }
}
