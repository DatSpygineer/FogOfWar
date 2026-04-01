#include "fow/Renderer/RenderQueue.hpp"

#include "fow/Renderer.hpp"

namespace fow::RenderQueue {
    struct Renderable {
        std::variant<MeshPtr, ModelPtr> model;
        std::variant<Transform, Vector<Transform>> transform;

        void draw(const Vector<LightInfoPtr>& lights) const;
    };

    static Deque<Renderable> s_render_queue;
    static Vector<LightInfoPtr> s_lights;
    static Vector4 s_sunlight_color = Vector4(0.0f);
    static bool s_sunlight_enabled = false;
    static const Transform* s_sunlight_transform = nullptr;
    static SkyboxPtr s_skybox = nullptr;
    static TextureCubeMapPtr s_envMap = nullptr;
    static TextureCubeMapPtr s_envMapBlur = nullptr;
    static float s_envMapIntensity = 1.0f;

    void Enqueue(const MeshPtr& mesh, const Transform& transform) {
        s_render_queue.push_back({ mesh, transform });
    }
    void Enqueue(const ModelPtr& model, const Transform& transform) {
        s_render_queue.push_back({ model, transform });
    }
    void EnqueueInstanced(const MeshPtr& mesh, const Vector<Transform>& transforms) {
        s_render_queue.push_back({ mesh, transforms });
    }
    void EnqueueInstanced(const ModelPtr& model, const Vector<Transform>& transforms) {
        s_render_queue.push_back({ model, transforms });
    }
    void SetSkybox(const SkyboxPtr& skybox) {
        s_skybox = skybox;
    }
    void SetEnvMap(const TextureCubeMapPtr& texture, const TextureCubeMapPtr& texture_blurred, const float intensity) {
        s_envMap = texture;
        s_envMapBlur = texture_blurred;
        s_envMapIntensity = intensity;
    }

    void SetSunlight(const Transform& transform, const Color& color, const float intensity, const bool is_enabled) {
        SetSunlight(transform, Vector4(color.to_vec3(), intensity), is_enabled);
    }
    void SetSunlight(const Transform& transform, const Vector3& color, const float intensity, const bool is_enabled) {
        SetSunlight(transform, Vector4(color, intensity), is_enabled);
    }
    void SetSunlight(const Transform& transform, const Vector4& color, const bool is_enabled) {
        s_sunlight_color = color;
        s_sunlight_transform = &transform;
        s_sunlight_enabled = is_enabled;
    }

    void SetSunlightColor(const Color& color, const float intensity) {
        s_sunlight_color = Vector4(color.to_vec3(), intensity);
    }
    void SetSunlightTransform(const Transform& transform) {
        s_sunlight_transform = &transform;
    }
    void SetSunlightEnabled(const bool enabled) {
        s_sunlight_enabled = enabled;
    }

    void RemoveSunlight() {
        s_sunlight_color = Vector4(0.0f);
        s_sunlight_transform = nullptr;
        s_sunlight_enabled = false;
    }

    LightInfoPtr AddLight(const Transform& transform, const Vector4& color, const bool is_enabled) {
        return AddLight(transform, Vector3(color.x, color.y, color.z), color.w, is_enabled);
    }
    LightInfoPtr AddLight(const Transform& transform, const Vector3& color, const float intensity, const bool is_enabled) {
        auto light_info = std::make_shared<LightInfo>(&transform, color, intensity, is_enabled);
        s_lights.push_back(light_info);
        return light_info;
    }
    LightInfoPtr AddLight(const Transform& transform, const Color& color, const float intensity, const bool is_enabled) {
        return AddLight(transform, Vector3(color.to_vec3()), intensity, is_enabled);
    }

    void RemoveLight(const LightInfoPtr& light) {
        if (const auto it = std::ranges::find(s_lights, light); it != s_lights.end()) {
            s_lights.erase(it);
        }
    }

    void Render() {
        if (s_skybox != nullptr) {
            s_skybox->draw();
        }
        while (!s_render_queue.empty()) {
            s_render_queue.front().draw(s_lights);
            s_render_queue.pop_front();
        }
    }

    static void applySceneParamsToMaterial(const MaterialPtr& mat, const Vector<LightInfoPtr>& lights) {
        if (mat == nullptr) {
            return;
        }

        size_t i = 0;
        for (const auto& light : lights) {
            mat->set_parameter(std::format("Lights[{}].Position", i), light->transform != nullptr ? light->transform->get_position() : Vector3Constants::Zero);
            mat->set_parameter(std::format("Lights[{}].Color", i), Vector4(light->color, light->intensity));
            ++i;
        }
        mat->set_parameter("LightCount", static_cast<GLuint>(lights.size()));
        mat->set_parameter("SunLightColor", s_sunlight_color);
        mat->set_parameter("SunLightDir", s_sunlight_transform->get_rotation() * Vector3Constants::Forward);
        mat->set_parameter("ViewPos", Renderer::GetCameraPosition());

        if (s_envMap != nullptr) {
            mat->set_parameter("EnvMap", s_envMap);
        }
        if (s_envMapBlur != nullptr) {
            mat->set_parameter("EnvMapBlur", s_envMapBlur);
        }
        mat->set_parameter("EnvMapStrength", s_envMapIntensity);
    }

    inline void Renderable::draw(const Vector<LightInfoPtr>& lights) const {
        if (model.index() == 0) {
            const auto mesh = std::get<0>(model);
            if (const auto mat = mesh->material(); mat != nullptr) {
                applySceneParamsToMaterial(mat, lights);
            }

            if (transform.index() == 0) {
                mesh->draw(std::get<0>(transform));
            } else {
                mesh->draw_instances(std::get<1>(transform));
            }
        } else {
            const auto mesh = std::get<1>(model);
            for (const auto mat : mesh->materials()) {
                applySceneParamsToMaterial(mat, lights);
            }

            if (transform.index() == 0) {
                mesh->draw(std::get<0>(transform));
            } else {
                mesh->draw_instances(std::get<1>(transform));
            }
        }
    }
}
