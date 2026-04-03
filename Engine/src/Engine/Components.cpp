#include "fow/Engine/Components.hpp"

#include "fow/Engine.hpp"
#include "fow/Renderer.hpp"

namespace fow {
    void TransformComponent::set_position(const Vector3& position) {
        m_transform.set_position(position);
    }
    void TransformComponent::set_rotation(const Vector3& rotation) {
        m_transform.set_rotation(rotation);
    }

    void TransformComponent::set_rotation_deg(const Vector3& rotation) {
        m_transform.set_rotation_deg(rotation);
    }

    void TransformComponent::set_rotation(const Quat& rotation) {
        m_transform.set_rotation(rotation);
    }
    void TransformComponent::set_scale(const Vector3& scale) {
        m_transform.set_scale(scale);
    }
    void TransformComponent::set_local_position(const Vector3& position) {
        m_transform.set_local_position(position);
    }
    void TransformComponent::set_local_rotation(const Vector3& rotation) {
        m_transform.set_local_rotation(rotation);
    }
    void TransformComponent::set_local_rotation(const Quat& rotation) {
        m_transform.set_local_rotation(rotation);
    }
    void TransformComponent::set_local_scale(const Vector3& scale) {
        m_transform.set_local_scale(scale);
    }
    void TransformComponent::set_parent(Transform& transform) {
        m_transform.set_parent(&transform);
    }

    Vector3 TransformComponent::get_position() const {
        return m_transform.get_position();
    }
    Quat TransformComponent::get_rotation() const {
        return m_transform.get_rotation();
    }
    Vector3 TransformComponent::get_scale() const {
        return m_transform.get_scale();
    }
    Vector3 TransformComponent::get_local_position() const {
        return m_transform.get_local_position();
    }
    Quat TransformComponent::get_local_rotation() const {
        return m_transform.get_local_rotation();
    }
    Vector3 TransformComponent::get_local_scale() const {
        return m_transform.get_local_scale();
    }
    const Transform& TransformComponent::get_parent() const {
        return *m_transform.get_parent();
    }

    void TransformComponent::set_transform(const Transform& transform) {
        m_transform = transform;
    }

    void TransformComponent::set_parameter(const String& name, const String& value) {
        if (name.equals("position", StringCompareType::CaseInsensitive)) {
            if (const auto result = StringToVec3(value); result.has_value()) {
                transform().set_position(result.value());
            } else {
                Debug::LogError(std::format("Failed to parse position value \"{}\": {}", value, result.error().message));
            }
        }
        if (name.equals("rotation", StringCompareType::CaseInsensitive)) {
            if (value.count(',') < 3) {
                if (const auto result = StringToVec3(value); result.has_value()) {
                    transform().set_rotation(result.value());
                } else {
                    Debug::LogError(std::format("Failed to parse rotation value \"{}\": {}", value, result.error().message));
                }
            } else {
                if (const auto result = StringToQuat(value); result.has_value()) {
                    transform().set_rotation(result.value());
                } else {
                    Debug::LogError(std::format("Failed to parse rotation value \"{}\": {}", value, result.error().message));
                }
            }
        }
        if (name.equals("scale", StringCompareType::CaseInsensitive)) {
            if (const auto result = StringToVec3(value); result.has_value()) {
                transform().set_scale(result.value());
            } else {
                Debug::LogError(std::format("Failed to parse scale value \"{}\": {}", value, result.error().message));
            }
        }
    }

    void EnvironmentComponent::on_spawn() {
        FOW_ASSERT_COMPONENT_DEPENDENCY_FATAL(EnvironmentComponent, TransformComponent);
        const auto transform = entity().get_component<TransformComponent>();
        RenderQueue::SetSunlight(transform->transform(), m_sunLightColor, m_sunLightIntensity, entity().is_enabled());
        RenderQueue::SetSkybox(m_pSkybox);
    }

    void EnvironmentComponent::on_destroy() {
        RenderQueue::RemoveSunlight();
    }

    void EnvironmentComponent::on_enable() {
        RenderQueue::SetSunlightEnabled(true);
        RenderQueue::SetSkybox(m_pSkybox);
        RenderQueue::SetEnvMap(m_pEnvMap, m_pEnvMapBlur, m_fEnvMapIntensity);
    }

    void EnvironmentComponent::on_disable() {
        RenderQueue::SetSunlightEnabled(false);
    }

    void EnvironmentComponent::set_sunlight_color(const Color& color) {
        m_sunLightColor = color;
        RenderQueue::SetSunlightColor(color, m_sunLightIntensity);
    }
    void EnvironmentComponent::set_sunlight_intensity(const float intensity) {
        m_sunLightIntensity = intensity;
        RenderQueue::SetSunlightColor(m_sunLightColor, intensity);
    }
    void EnvironmentComponent::set_skybox(const SkyboxPtr& skybox) {
        m_pSkybox = skybox;
        RenderQueue::SetSkybox(skybox);
    }

    void EnvironmentComponent::set_env_map(const TextureCubeMapPtr& texture, const TextureCubeMapPtr& texture_blurred, float intensity) {
        m_pEnvMap = texture;
        m_pEnvMapBlur = texture_blurred;
        m_fEnvMapIntensity = intensity;
        RenderQueue::SetEnvMap(texture, texture_blurred, intensity);
    }

    void EnvironmentComponent::set_parameter(const String& name, const String& value) {
        if (name.equals("sunlight_color", StringCompareType::CaseInsensitive)) {
            if (const auto result = StringToColor(value); result.has_value()) {
                m_sunLightColor = result.value();
            } else {
                Debug::LogError(std::format("Failed to parse color value \"{}\": {}", value, result.error().message));
            }
        }
        if (name.equals("sunlight_intensity", StringCompareType::CaseInsensitive)) {
            m_sunLightIntensity = StringToFloat<float>(value).value_or(1.0f);
        }
        if (name.equals("skybox", StringCompareType::CaseInsensitive)) {
            if (const auto skybox = Assets::Load<Skybox>(value); skybox.has_value()) {
                m_pSkybox = skybox.value().ptr();
            }
        }
        if (name.equals("env_map", StringCompareType::CaseInsensitive)) {
            if (const auto texture = Assets::Load<TextureCubeMap>(value); texture.has_value()) {
                m_pEnvMap = texture.value().ptr();
            }
        }
        if (name.equals("env_map_blur", StringCompareType::CaseInsensitive)) {
            if (const auto texture = Assets::Load<TextureCubeMap>(value); texture.has_value()) {
                m_pEnvMapBlur = texture.value().ptr();
            }
        }
        if (name.equals("env_map_intensity", StringCompareType::CaseInsensitive)) {
            m_fEnvMapIntensity = StringToFloat<float>(value).value_or(1.0f);
        }
    }

    void LightComponent::on_spawn() {
        FOW_ASSERT_COMPONENT_DEPENDENCY_FATAL(LightComponent, TransformComponent);
        const auto transform = entity().get_component<TransformComponent>();
        m_pLightInfo = RenderQueue::AddLight(transform->transform(), m_color, m_intensity, entity().is_enabled());
    }

    void LightComponent::on_destroy() {
        if (m_pLightInfo != nullptr) {
            RenderQueue::RemoveLight(m_pLightInfo);
        }
    }

    void LightComponent::on_enable() {
        if (m_pLightInfo != nullptr) {
            m_pLightInfo->enabled = true;
        } else {
            m_pLightInfo = RenderQueue::AddLight(entity().get_component<TransformComponent>()->transform(), m_color, m_intensity, true);
        }
    }
    void LightComponent::on_disable() {
        if (m_pLightInfo != nullptr) {
            m_pLightInfo->enabled = false;
        }
    }

    void LightComponent::set_color(const Color& color) {
        m_color = color;
        if (m_pLightInfo != nullptr) {
            m_pLightInfo->color = color.to_vec3();
        }
    }
    void LightComponent::set_intensity(const float intensity) {
        m_intensity = intensity;
        if (m_pLightInfo != nullptr) {
            m_pLightInfo->intensity = intensity;
        }
    }

    void LightComponent::set_parameter(const String& name, const String& value) {
        if (name.equals("color", StringCompareType::CaseInsensitive)) {
            if (const auto result = StringToColor(value); result.has_value()) {
                m_color = result.value();
            } else {
                Debug::LogError(std::format("Failed to parse color value \"{}\": {}", value, result.error().message));
            }
        }
        if (name.equals("intensity", StringCompareType::CaseInsensitive)) {
            m_intensity = StringToFloat<float>(value).value_or(1.0f);
        }
    }

    void CameraComponent::on_spawn() {
        FOW_ASSERT_COMPONENT_DEPENDENCY_FATAL(CameraComponent, TransformComponent);
    }
    void CameraComponent::on_update(const double dt) {
        const auto transform = entity().get_component<TransformComponent>();
        Renderer::UpdateCameraPosition(transform->transform().get_position(), Vector3Constants::Forward, Vector3Constants::Up, transform->transform().get_rotation());
        Renderer::UpdateCameraProjectionPerspective(m_fFov, Engine::GetWindowSize(), m_fNear, m_fFar);
    }

    void CameraComponent::set_fov(const float value) {
        m_fFov = value;
    }
    void CameraComponent::set_near_clipping(const float value) {
        m_fNear = value;
    }
    void CameraComponent::set_far_clipping(const float value) {
        m_fFar = value;
    }

    void CameraComponent::set_parameter(const String& name, const String& value) {
        if (name.equals("fov", StringCompareType::CaseInsensitive)) {
            m_fFov = StringToFloat<float>(value).value_or(60.0f);
        }
        if (name.equals("near", StringCompareType::CaseInsensitive)) {
            m_fNear = StringToFloat<float>(value).value_or(0.1f);
        }
        if (name.equals("far", StringCompareType::CaseInsensitive)) {
            m_fFar = StringToFloat<float>(value).value_or(1000.0f);
        }
    }

    void SpriteRendererComponent::on_spawn() {
        FOW_ASSERT_COMPONENT_DEPENDENCY_FATAL(ModelRendererComponent, TransformComponent);
    }

    void SpriteRendererComponent::on_update(const double dt) {
        const auto transform = entity().get_component<TransformComponent>();
        RenderQueue::Enqueue(m_pSprite, transform->transform());
    }

    void SpriteRendererComponent::set_sprite(const SpritePtr& sprite) {
        m_pSprite = sprite;
    }

    void ModelRendererComponent::on_spawn() {
        FOW_ASSERT_COMPONENT_DEPENDENCY_FATAL(ModelRendererComponent, TransformComponent);
    }
    void ModelRendererComponent::on_update(double dt) {
        const auto transform = entity().get_component<TransformComponent>();
        RenderQueue::Enqueue(m_pModel, transform->transform());
    }

    void ModelRendererComponent::set_model(const ModelPtr& model) {
        m_pModel = model;
    }
    bool ModelRendererComponent::load_model(const Path& path) {
        auto model_result = Assets::Load<Model>(path);
        if (model_result.has_value()) {
            m_pModel = model_result.value().ptr();
            return true;
        }

        Debug::LogError(std::format("Failed to load model \"{}\": {}", path, model_result.error().message));
        return false;
    }

    void ModelRendererComponent::set_parameter(const String& name, const String& value) {
        if (name.equals("model", StringCompareType::CaseInsensitive)) {
            if (const auto model = Assets::Load<Model>(value); model.has_value()) {
                m_pModel = model.value().ptr();
            } else {
                Debug::LogError(std::format("Failed to load model \"{}\": {}", value, model.error().message));
            }
        }
    }
}
