#version 460 core

#define MAX_LIGHTS 32

struct PhongParams {
    vec4 Albedo;
    float PhongStrength;
    float PhongExponent;
    float EnvMapStrength;
};
struct PhongPointLightInfo {
    vec3 Position;
    vec3 Color;

    float Constant;
    float Linear;
    float Quadratic;
};
struct EnvironmentInfo {
    vec3 AmbientColor;
    float AmbientStrength;

    vec3 SunDirection;
    vec3 SunLightColor;
};

uniform PhongPointLightInfo Lights[MAX_LIGHTS];
uniform int LightCount;
uniform EnvironmentInfo Environment;

uniform float PhongStrength = 1.0;
uniform float PhongExponent = 1.0;
uniform float EnvMapStrength = 0.5;

vec4 light_directional(EnvironmentInfo env, PhongParams phong_params, vec3 norm, vec3 view_dir) {
    vec3 light_dir = normalize(-env.SunDirection);
    float diff = max(dot(norm, light_dir), 0.0);
    vec3 refl_dir = reflect(-light_dir, norm);
    float spec = pow(max(dot(view_dir, refl_dir), 0.0), phong_params.PhongExponent);

    vec3 ambient  = env.AmbientColor * env.AmbientStrength * phong_params.Albedo.rgb;
    vec3 diffuse  = env.SunLightColor * diff * phong_params.Albedo.rgb;
    vec3 specular = env.SunLightColor * spec * phong_params.PhongStrength;

    return vec4(ambient + diffuse + specular, spec * phong_params.PhongStrength);
}

vec4 light_point(PhongPointLightInfo light_info, EnvironmentInfo env, PhongParams phong_params, vec3 norm, vec3 world_pos, vec3 view_dir) {
    vec3 light_dir = normalize(light_info.Position - world_pos);

    // Diffuse
    float diff = max(dot(norm, light_dir), 0.0);

    // Specular
    vec3 refl_dir = reflect(-light_dir, norm);
    float spec = pow(max(dot(view_dir, refl_dir), 0.0), phong_params.PhongExponent);

    // Attenuation
    float dist  = length(light_info.Position - world_pos);
    float atten = 1.0 / (light_info.Constant + light_info.Linear * dist + light_info.Quadratic * (dist * dist));

    vec3 ambient  = env.AmbientColor * env.AmbientStrength * phong_params.Albedo.rgb * atten;
    vec3 diffuse  = light_info.Color * diff * phong_params.Albedo.rgb * atten;
    float spec_h  = spec * phong_params.PhongStrength * atten;
    vec3 specular = light_info.Color * spec_h;

    return vec4(ambient + diffuse + specular, spec_h);
}
vec3 envmap_refl(samplerCube envmap, PhongParams params, vec3 norm, vec3 view_pos, vec3 world_pos) {
    vec3 i = normalize(world_pos - view_pos);
    vec3 r = reflect(i, norm);
    return texture(envmap, r).rgb * params.EnvMapStrength;
}

uniform sampler2D MainTexture;
uniform sampler2D NormalMap;
uniform sampler2D SpecularMap;
uniform sampler2D SelfIllumMask;
uniform samplerCube EnvMap;
uniform vec3  ViewPos;
uniform bool  AlphaScissor = false;
uniform float AlphaScissorThreshold = 0.5;

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

    float alpha = albedo.a;
    if (AlphaScissor && alpha < AlphaScissorThreshold) {
        discard;
    }

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

    FRAGMENT_COLOR = vec4(mix(color.rgb, color.rgb + albedo.rgb, selfillum), alpha);
}
