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
};

uniform sampler2D MainTexture;
uniform sampler2D NormalMap;
uniform sampler2D SpecularMap;
uniform samplerCube EnvMap;
uniform vec3  ViewPos;
uniform float PhongStrength = 1.0;
uniform float PhongExponent = 1.0;
uniform float EnvMapStrength = 0.5;

uniform PhongPointLightInfo Lights[MAX_LIGHTS];
uniform int LightCount;

uniform EnvironmentInfo Environment;

in vec3 FRAGMENT_WORLD_POSITION;
in vec2 FRAGMENT_TEXTURE_COORDS;
in vec3 FRAGMENT_NORMAL;
in vec3 FRAGMENT_WORLD_NORMAL;

out vec4 FRAGMENT_COLOR;

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
    float specular_highlight = spec * phong_params.PhongStrength * atten;
    vec3 specular = light_info.Color * specular_highlight;

    return vec4(ambient + diffuse + specular, specular_highlight);
}
vec3 envmap_refl(PhongParams params, vec3 norm) {
    vec3 i = normalize(FRAGMENT_WORLD_POSITION - ViewPos);
    vec3 r = reflect(i, norm);
    return texture(EnvMap, r).rgb * params.EnvMapStrength;
}

void main() {
    vec3 norm = normalize(FRAGMENT_WORLD_NORMAL);
    vec3 view_dir = normalize(ViewPos - FRAGMENT_WORLD_POSITION);

    vec4 albedo  = texture(MainTexture, FRAGMENT_TEXTURE_COORDS);
    vec3 specmap = texture(SpecularMap, FRAGMENT_TEXTURE_COORDS).xyz;
    PhongParams params;
    params.Albedo = albedo;
    params.PhongStrength  = PhongStrength * specmap.x;
    params.PhongExponent  = clamp(PhongExponent * specmap.y, 0.01, 1.0) * 256;
    params.EnvMapStrength = EnvMapStrength * specmap.z;
    params.Albedo += vec4(envmap_refl(params, norm), 1.0);

    vec4 color = vec4(0.0);
    for (int i = 0; i < LightCount; ++i) {
        color += light_point(Lights[i], Environment, params, norm, FRAGMENT_WORLD_POSITION, view_dir);
    }

    FRAGMENT_COLOR = vec4(color.rgb, min(color.a + albedo.a, 1.0));
}
