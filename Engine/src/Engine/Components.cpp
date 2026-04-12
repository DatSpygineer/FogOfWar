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

    void Transform2DComponent::set_rectangle(const Rectangle& rectangle) {
        m_rectangle = rectangle;
    }

    void Transform2DComponent::set_rotation(const float angle_rad) {
        m_fRotation = angle_rad;
    }
    void Transform2DComponent::set_rotation_deg(const float angle_deg) {
        set_rotation(glm::radians(angle_deg));
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
                set_color(result.value());
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

    void SpriteRendererComponent::set_parameter(const String& name, const String& value) {
        if (name.equals("sprite", StringCompareType::CaseInsensitive)) {
            auto spr = Assets::Load<Sprite>(value);
            Debug::Assert(spr);
            if (spr.has_value()) {
                m_pSprite = spr.value().ptr();
            }
        }
    }

    void TextRendererComponent::on_spawn() {
        FOW_ASSERT_COMPONENT_DEPENDENCY_FATAL(TextRendererComponent, TransformComponent);
        if (m_pText == nullptr) {
            const auto transform = entity().get_component<TransformComponent>();

            if (m_pMaterial == nullptr) {
                if (const auto material = Material::New("Sprite"); material.has_value()) {
                    m_pMaterial = material.value();
                } else {
                    Debug::LogError("Failed to create material with shader \"Sprite\"");
                }
            }

            m_pText = CreateRef<TextSprite>(m_sText, m_pFont, m_pMaterial, m_TextRect);
            m_pText->set_text_wrap_width(m_iTextWrapWidth);
            m_pText->set_billboard_mode(m_eBillboardMode);
        }
    }

    void TextRendererComponent::on_update(double dt) {
        if (m_pText == nullptr) return;
        const auto transform = entity().get_component<TransformComponent>();
        RenderQueue::Enqueue(m_pText, transform->transform());
    }

    void TextRendererComponent::set_material(const MaterialPtr& material) {
        m_pMaterial = material;
        if (m_pText == nullptr) return;
        m_pText->set_material(m_pMaterial);
    }

    void TextRendererComponent::set_font(const Font& font) {
        m_pFont = CreateRef<Font>(font);
        if (m_pText == nullptr) return;
        m_pText->set_font(m_pFont);
    }
    void TextRendererComponent::set_font(const FontPtr& font) {
        m_pFont = font;
        if (m_pText == nullptr) return;
        m_pText->set_font(m_pFont);
    }

    void TextRendererComponent::set_billboard_mode(const BillboardMode mode) {
        m_eBillboardMode = mode;
        if (m_pText == nullptr) return;
        m_pText->set_billboard_mode(m_eBillboardMode);
    }

    void TextRendererComponent::set_text(const String& text) {
        m_sText = text;
        if (m_pText == nullptr) return;
        m_pText->set_text(m_sText);
    }

    String TextRendererComponent::get_text() const {
        return m_pText != nullptr ? m_pText->text() : m_sText;
    }

    void TextRendererComponent::set_text_wrap_width(const int width) {
        m_iTextWrapWidth = width;
        if (m_pText == nullptr) return;
        m_pText->set_text_wrap_width(m_iTextWrapWidth);
    }

    int TextRendererComponent::get_text_wrap_width() const {
        return m_pText != nullptr ? m_pText->text_wrap_width() : m_iTextWrapWidth;
    }

    void TextRendererComponent::set_text_sprite(const TextSpritePtr& text) {
        m_pText = text;
        m_pMaterial = m_pText->material();
        m_iTextWrapWidth = m_pText->text_wrap_width();
        m_sText = m_pText->text();
    }

    void TextRendererComponent::set_text_rect(const IntRectangle& rect) {
        m_TextRect = rect;
        if (m_pText == nullptr) return;
        m_pText->set_text_area(m_TextRect);
    }

    void TextRendererComponent::set_parameter(const String& name, const String& value) {
        if (name.equals("font", StringCompareType::CaseInsensitive)) {
            Path path;
            float size;
            if (value.find(';') != String::NotFound) {
                path = value.substr(0, value.find(';'));
                size = StringToFloat<float>(value.substr(value.find(';') + 1)).value_or(12.0f);
            } else {
                path = value;
                size = 12.0f;
            }
            m_pFont = CreateRef<Font>(path, size);
        }
        if (name.equals("text", StringCompareType::CaseInsensitive)) {
            m_sText = value;
        }
        if (name.equals("text_wrap_width", StringCompareType::CaseInsensitive)) {
            m_iTextWrapWidth = StringToInt<int>(value).value_or(0);
        }
        if (name.equals("material", StringCompareType::CaseInsensitive)) {
            auto mat = Assets::Load<Material>(value);
            Debug::Assert(mat);
            if (mat.has_value()) {
                m_pText->set_material(mat.value().ptr());
            }
        }
        if (name.equals("billboard_mode", StringCompareType::CaseInsensitive)) {
            if (value.equals_any({ "yaligned", "y_aligned", "cylindrical" }, StringCompareType::CaseInsensitive)) {
                m_eBillboardMode = BillboardMode::BillboardCylindrical;
            } else if (value.equals_any({ "spherical" }, StringCompareType::CaseInsensitive)) {
                m_eBillboardMode = BillboardMode::BillboardSpherical;
            } else {
                m_eBillboardMode = BillboardMode::None;
            }
        }
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

    void Sprite2DRendererComponent::on_spawn() {
        FOW_ASSERT_COMPONENT_DEPENDENCY_FATAL(Sprite2DRendererComponent, Transform2DComponent);
    }

    void Sprite2DRendererComponent::on_update(const double dt) {
        const auto transform = entity().get_component<Transform2DComponent>();
        RenderQueue2D::Enqueue(m_pSprite, transform->get_rectangle());
    }

    void Sprite2DRendererComponent::set_sprite(const Sprite2DPtr& sprite) {
        m_pSprite = sprite;
    }

    void Sprite2DRendererComponent::set_parameter(const String& name, const String& value) {
        if (name.equals("sprite", StringCompareType::CaseInsensitive)) {
            auto spr = Assets::Load<Sprite2D>(value);
            Debug::Assert(spr);
            if (spr.has_value()) {
                m_pSprite = spr.value().ptr();
            }
        }
    }

    void Text2DRendererComponent::on_spawn() {
        FOW_ASSERT_COMPONENT_DEPENDENCY_FATAL(Text2DRendererComponent, Transform2DComponent);
        if (m_pText == nullptr) {
            const auto transform = entity().get_component<Transform2DComponent>();
            if (m_pMaterial == nullptr) {
                if (const auto material = Material::New("Generic2D"); material.has_value()) {
                    m_pMaterial = material.value();
                } else {
                    Debug::LogError("Failed to create material with shader \"Generic2D\"");
                }
            }
            m_pText = CreateRef<TextSprite2D>(m_sText, m_pFont, m_pMaterial, static_cast<IntRectangle>(transform->get_rectangle()));
            m_pText->set_text_wrap_width(m_iTextWrapWidth);
        }
    }

    void Text2DRendererComponent::on_update(const double dt) {
        if (m_pText == nullptr) return;
        const auto transform = entity().get_component<Transform2DComponent>();
        RenderQueue2D::Enqueue(m_pText, transform->get_rectangle());
    }

    void Text2DRendererComponent::set_material(const MaterialPtr& material) {
        m_pMaterial = material;
        if (m_pText == nullptr) return;
        m_pText->set_material(m_pMaterial);
    }

    void Text2DRendererComponent::set_font(const Font& font) {
        m_pFont = CreateRef<Font>(font);
        if (m_pText == nullptr) return;
        m_pText->set_font(m_pFont);
    }
    void Text2DRendererComponent::set_font(const FontPtr& font) {
        m_pFont = font;
        if (m_pText == nullptr) return;
        m_pText->set_font(m_pFont);
    }

    void Text2DRendererComponent::set_text(const String& text) {
        m_sText = text;
        if (m_pText == nullptr) return;
        m_pText->set_text(m_sText);
    }
    String Text2DRendererComponent::get_text() const {
        return m_pText != nullptr ? m_pText->text() : m_sText;
    }

    void Text2DRendererComponent::set_text_wrap_width(const int width) {
        m_iTextWrapWidth = width;
        if (m_pText == nullptr) return;
        m_pText->set_text_wrap_width(m_iTextWrapWidth);
    }
    int Text2DRendererComponent::get_text_wrap_width() const {
        return m_pText != nullptr ? m_pText->text_wrap_width() : m_iTextWrapWidth;
    }

    void Text2DRendererComponent::set_text_sprite(const TextSprite2DPtr& text) {
        m_pText = text;
        m_pMaterial = m_pText->material();
        m_iTextWrapWidth = m_pText->text_wrap_width();
        m_sText = m_pText->text();
    }

    void Text2DRendererComponent::set_parameter(const String& name, const String& value) {
        if (name.equals("font", StringCompareType::CaseInsensitive)) {
            Path path;
            float size;
            if (value.find(';') != String::NotFound) {
                path = value.substr(0, value.find(';'));
                size = StringToFloat<float>(value.substr(value.find(';') + 1)).value_or(12.0f);
            } else {
                path = value;
                size = 12.0f;
            }
            m_pFont = CreateRef<Font>(path, size);
        }

        if (name.equals("text", StringCompareType::CaseInsensitive)) {
            m_sText = value;
        }
        if (name.equals("text_wrap_width", StringCompareType::CaseInsensitive)) {
            m_iTextWrapWidth = StringToInt<int>(value).value_or(0);
        }
        if (name.equals("material", StringCompareType::CaseInsensitive)) {
            auto mat = Assets::Load<Material>(value);
            Debug::Assert(mat);
            if (mat.has_value()) {
                m_pText->set_material(mat.value().ptr());
            }
        }
    }
}
