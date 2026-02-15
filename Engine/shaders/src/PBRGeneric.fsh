#version 460 core

#define MAX_LIGHTS 32

const float PI = 3.14159265359;

struct PBRLightInfo {
    vec3 Position;
    vec4 Color;
};

uniform sampler2D MainTexture;
uniform sampler2D NormalMap;
uniform sampler2D SpecularMap;
uniform sampler2D SelfIllumMask;
uniform sampler2D BrdfLut;
uniform samplerCube EnvMap;
uniform samplerCube EnvMapBlur;
uniform vec3 ViewPos;
uniform vec4 ColorTint = vec4(1.0);
uniform bool  AlphaScissor = false;
uniform float AlphaScissorThreshold = 0.5;
uniform PBRLightInfo Lights[MAX_LIGHTS];
uniform int LightCount;
uniform float EnvMapStrength;

uniform float Roughness        = 1.0;
uniform float Metallicness     = 1.0;
uniform float AmbientOcclusion = 1.0;

in vec3 FRAGMENT_WORLD_POSITION;
in vec2 FRAGMENT_TEXTURE_COORDS;
in vec3 FRAGMENT_NORMAL;
in mat3 FRAGMENT_TBN;

out vec4 FRAGMENT_COLOR;

vec3 extractNormalFromNormalMap() {
    vec3 tangentNormal = texture(NormalMap, FRAGMENT_TEXTURE_COORDS).xyz * 2.0 - 1.0;

    vec3 Q1  = dFdx(FRAGMENT_WORLD_POSITION);
    vec3 Q2  = dFdy(FRAGMENT_WORLD_POSITION);
    vec2 st1 = dFdx(FRAGMENT_TEXTURE_COORDS);
    vec2 st2 = dFdy(FRAGMENT_TEXTURE_COORDS);

    vec3 N   = normalize(FRAGMENT_NORMAL);
    vec3 T  = normalize(Q1 * st2.t - Q2 * st1.t);
    vec3 B  = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}

float distributionGGX(vec3 normal, vec3 halfway, float roughness) {
    float a = roughness * roughness;
    float a2 = a * a;
    float ndoth = max(dot(normal, halfway), 0.0);
    float ndoth2 = ndoth * ndoth;

    float nom = a2;
    float denom = (ndoth2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}
float geometrySchlickGGX(float ndotv, float roughness) {
    float r = roughness + 1.0;
    float k = r * r / 8.0;

    float nom   = ndotv;
    float denom = ndotv * (1.0 - k) + k;

    return nom / denom;
}
float geometrySmith(vec3 normal, vec3 view, vec3 l, float roughness) {
    float ndotv = max(dot(normal, view), 0.0);
    float ndotl = max(dot(normal, l), 0.0);
    float ggx2 = geometrySchlickGGX(ndotv, roughness);
    float ggx1 = geometrySchlickGGX(ndotl, roughness);

    return ggx1 * ggx2;
}
vec3 fresnelSchlick(float cos_theta, vec3 f0) {
    return f0 + (1.0 - f0) * pow(clamp(1.0 - cos_theta, 0.0, 1.0), 5.0);
}
vec3 fresnelSchlickRoughness(float cos_theta, vec3 f0, float roughness) {
    return f0 + (max(vec3(1.0 - roughness), f0) - f0) * pow(clamp(1.0 - cos_theta, 0.0, 1.0), 5.0);
}

vec3 pbr_light(vec3 normal, vec3 view, vec3 base_reflectivity, vec3 albedo, float metallic, float roughness) {
    vec3 light_result = vec3(0.0);
    for (int i = 0; i < LightCount; i++) {
        vec3 l = normalize(Lights[i].Position - FRAGMENT_WORLD_POSITION);
        vec3 h = normalize(view + l);
        float distance = length(Lights[i].Position - FRAGMENT_WORLD_POSITION);
        float atten = 1.0 / (distance * distance);
        vec3 radiance = Lights[i].Color.rgb * Lights[i].Color.a * atten;

        float ndf = distributionGGX(normal, h, roughness);
        float g = geometrySmith(normal, view, l, roughness);
        vec3  f = fresnelSchlick(clamp(dot(h, view), 0.0, 1.0), base_reflectivity);

        vec3 numerator = ndf * g * f;
        float denominator = 4.0 * max(dot(normal, view), 0.0) * max(dot(normal, view), 0.0) + 0.0001;
        vec3 specular = numerator / denominator;

        vec3 ks = f;
        vec3 kd = vec3(1.0) - ks;
        kd *= 1.0 - metallic;

        float ndotl = max(dot(normal, l), 0.0);
        light_result += (kd * albedo / PI + specular) * radiance * ndotl;
    }
    return light_result;
}

void main() {
    vec3 normal = extractNormalFromNormalMap();
    vec3 view = normalize(ViewPos - FRAGMENT_WORLD_POSITION);
    vec3 refl = reflect(-view, normal);

    vec4 mainTex   = texture(MainTexture, FRAGMENT_TEXTURE_COORDS) * ColorTint;
    vec3 albedo    = pow(mainTex.rgb, vec3(2.2));
    vec4 specular  = texture(SpecularMap, FRAGMENT_TEXTURE_COORDS);
    float metallic  = specular.r * Metallicness;
    float roughness = clamp(specular.g * Roughness, 0.05, 0.9);
    float envmap_mask = normalize(specular.b) * EnvMapStrength;
    float ao        = specular.a * AmbientOcclusion;

    vec3 base_reflectivity = vec3(0.04);

    float alpha = mainTex.a;
    if (AlphaScissor && alpha < AlphaScissorThreshold) {
        discard;
    }

    base_reflectivity = mix(base_reflectivity, albedo, metallic);
    vec3 f = fresnelSchlickRoughness(max(dot(normal, view), 0.0), base_reflectivity, roughness);
    vec3 ks = f;
    vec3 kd = 1.0 - ks;
    kd *= 1.0 - metallic;
    vec3 envmap = texture(EnvMap, normal).rgb;
    vec3 diffuse = albedo;

    vec3 light = pbr_light(normal, view, base_reflectivity, albedo, metallic, roughness);

    vec3 prefilteredColor = texture(EnvMapBlur, refl).rgb;
    vec2 brdf = texture(BrdfLut, vec2(max(dot(normal, view), 0.0), roughness)).rg;
    vec3 envmap_specular = prefilteredColor * (f * brdf.x + brdf.y) * roughness * envmap_mask;

    diffuse *= envmap * (1.0 - roughness) * envmap_mask;

    vec3 amb = (kd * diffuse + envmap_specular) * ao;
    vec3 color = amb + light;

    // HDR tonemapping
    color = color / (color + vec3(1.0));
    // gamma correction
    color = pow(color, vec3(1.0 / 2.2));

    FRAGMENT_COLOR = vec4(color, alpha);
}