#ifndef FOW_ENGINE_COMPONENTS_HPP
#define FOW_ENGINE_COMPONENTS_HPP

#include "fow/Engine/Entity.hpp"

namespace fow {
    class FOW_ENGINE_API TransformComponent : public Component {
        Transform m_transform;
    public:
        FOW_COMPONENT_CLASS(TransformComponent, Component)

        FOW_CONSTEXPR Transform& transform() { return m_transform; }
        FOW_CONSTEXPR const Transform& transform() const { return m_transform; }

        void set_position(const Vector3& position);
        void set_rotation(const Vector3& rotation);
        void set_rotation_deg(const Vector3& rotation);
        void set_rotation(const Quat& rotation);
        void set_scale(const Vector3& scale);

        void set_local_position(const Vector3& position);
        void set_local_rotation(const Vector3& rotation);
        void set_local_rotation(const Quat& rotation);
        void set_local_scale(const Vector3& scale);

        void set_parent(Transform& transform);

        Vector3 get_position() const;
        Quat get_rotation() const;
        Vector3 get_scale() const;
        Vector3 get_local_position() const;
        Quat get_local_rotation() const;
        Vector3 get_local_scale() const;

        [[nodiscard]] FOW_CONSTEXPR Vector3 get_forward() const { return transform().get_forward(); }
        [[nodiscard]] FOW_CONSTEXPR Vector3 get_backward() const { return transform().get_backward(); }
        [[nodiscard]] FOW_CONSTEXPR Vector3 get_up() const { return transform().get_up(); }
        [[nodiscard]] FOW_CONSTEXPR Vector3 get_down() const { return transform().get_down(); }
        [[nodiscard]] FOW_CONSTEXPR Vector3 get_left() const { return transform().get_left(); }
        [[nodiscard]] FOW_CONSTEXPR Vector3 get_right() const { return transform().get_right(); }

        const Transform& get_parent() const;

        void set_transform(const Transform& transform);

        void set_parameter(const String& name, const String& value) override;
    };

    class FOW_ENGINE_API Transform2DComponent : public Component {
        Rectangle m_rectangle;
        float m_fRotation = 0.0f;
    public:
        FOW_COMPONENT_CLASS(Transform2DComponent, Component);

        void set_rectangle(const Rectangle& rectangle);
        [[nodiscard]] FOW_CONSTEXPR Rectangle& get_rectangle() { return m_rectangle; }
        [[nodiscard]] FOW_CONSTEXPR const Rectangle& get_rectangle() const { return m_rectangle; }

        void set_rotation(float angle_rad);
        void set_rotation_deg(float angle_deg);
        [[nodiscard]] FOW_CONSTEXPR float get_rotation() const { return m_fRotation; }
        [[nodiscard]] FOW_CONSTEXPR float get_rotation_deg() const { return glm::degrees(m_fRotation); }
    };

    class FOW_ENGINE_API EnvironmentComponent : public Component {
        SkyboxPtr m_pSkybox;
        TextureCubeMapPtr m_pEnvMap, m_pEnvMapBlur;
        float m_fEnvMapIntensity;
        Color m_sunLightColor;
        float m_sunLightIntensity;
    public:
        FOW_COMPONENT_CLASS(EnvironmentComponent, Component)

        void on_spawn() override;
        void on_destroy() override;

        void on_enable() override;
        void on_disable() override;

        void set_sunlight_color(const Color& color);
        void set_sunlight_intensity(float intensity);
        void set_skybox(const SkyboxPtr& skybox);
        void set_env_map(const TextureCubeMapPtr& texture, const TextureCubeMapPtr& texture_blurred, float intensity);

        [[nodiscard]] FOW_CONSTEXPR const Color& sunlight_color() const { return m_sunLightColor; }
        [[nodiscard]] FOW_CONSTEXPR float sunlight_intensity() const { return m_sunLightIntensity; }
        [[nodiscard]] FOW_CONSTEXPR const SkyboxPtr& skybox() const { return m_pSkybox; }

        void set_parameter(const String& name, const String& value) override;
    };

    class FOW_ENGINE_API LightComponent : public Component {
        Color m_color = ColorConstants::White;
        float m_intensity = 1.0f;
        LightInfoPtr m_pLightInfo = nullptr;
    public:
        FOW_COMPONENT_CLASS(LightComponent, Component)

        void on_spawn() override;
        void on_destroy() override;

        void on_enable() override;
        void on_disable() override;

        void set_color(const Color& color);
        void set_intensity(float intensity);

        [[nodiscard]] FOW_CONSTEXPR const Color& color() const { return m_color; }
        [[nodiscard]] FOW_CONSTEXPR float intensity() const { return m_intensity; }

        void set_parameter(const String& name, const String& value) override;
    };

    class FOW_ENGINE_API CameraComponent : public Component {
        float m_fFov = 60.0f, m_fNear = 0.1f, m_fFar = 1000.0f;
    public:
        FOW_COMPONENT_CLASS(CameraComponent, Component)

        void on_spawn() override;
        void on_update(double dt) override;

        void set_fov(float value);
        void set_near_clipping(float value);
        void set_far_clipping(float value);

        FOW_CONSTEXPR float fov() const { return m_fFov; }
        FOW_CONSTEXPR float near_clipping() const { return m_fNear; }
        FOW_CONSTEXPR float far_clipping() const { return m_fFar; }

        void set_parameter(const String& name, const String& value) override;
    };

    class FOW_ENGINE_API SpriteRendererComponent : public Component {
        SpritePtr m_pSprite;
    public:
        FOW_COMPONENT_CLASS(SpriteRendererComponent, Component)

        void on_spawn() override;
        void on_update(double dt) override;

        void set_sprite(const SpritePtr& sprite);
        [[nodiscard]] FOW_CONSTEXPR SpritePtr& get_sprite() { return m_pSprite; }
        [[nodiscard]] FOW_CONSTEXPR const SpritePtr& get_sprite() const { return m_pSprite; }

        void set_parameter(const String& name, const String& value) override;
    };

    class FOW_ENGINE_API TextRendererComponent : public Component {
        TextSpritePtr m_pText;
        FontPtr m_pFont;
        String m_sText = "Sample Text";
        int m_iTextWrapWidth = 0;
        MaterialPtr m_pMaterial;
        IntRectangle m_TextRect = { 0, 0, 128, 128 };
        BillboardMode m_eBillboardMode = BillboardMode::None;
    public:
        FOW_COMPONENT_CLASS(TextRendererComponent, Component)

        void on_spawn() override;
        void on_update(double dt) override;

        void set_material(const MaterialPtr& material);
        [[nodiscard]] FOW_CONSTEXPR const MaterialPtr& get_material() const { return m_pMaterial; }

        void set_font(const Font& font);
        void set_font(const FontPtr& font);
        [[nodiscard]] FOW_CONSTEXPR const FontPtr& get_font() const { return m_pFont; }

        void set_billboard_mode(BillboardMode mode);
        [[nodiscard]] FOW_CONSTEXPR BillboardMode get_billboard_mode() const { return m_eBillboardMode; }

        void set_text(const String& text);
        [[nodiscard]] String get_text() const;

        void set_text_wrap_width(int width);
        [[nodiscard]] int get_text_wrap_width() const;

        void set_text_sprite(const TextSpritePtr& text);
        [[nodiscard]] FOW_CONSTEXPR const TextSpritePtr& get_text_sprite() const { return m_pText; }

        void set_text_rect(const IntRectangle& rect);
        [[nodiscard]] FOW_CONSTEXPR const IntRectangle& get_text_rect() const { return m_TextRect; }

        void set_parameter(const String& name, const String& value) override;
    };

    class FOW_ENGINE_API ModelRendererComponent : public Component {
        ModelPtr m_pModel;
    public:
        FOW_COMPONENT_CLASS(ModelRendererComponent, Component)

        void on_spawn() override;
        void on_update(double dt) override;

        void set_model(const ModelPtr& model);
        bool load_model(const Path& path);

        void set_parameter(const String& name, const String& value) override;
    };

    class FOW_ENGINE_API Sprite2DRendererComponent : public Component {
        Sprite2DPtr m_pSprite;
    public:
        FOW_COMPONENT_CLASS(Sprite2DRendererComponent, Component)

        void on_spawn() override;
        void on_update(double dt) override;

        void set_sprite(const Sprite2DPtr& sprite);
        [[nodiscard]] FOW_CONSTEXPR Sprite2DPtr& get_sprite() { return m_pSprite; }
        [[nodiscard]] FOW_CONSTEXPR const Sprite2DPtr& get_sprite() const { return m_pSprite; }

        void set_parameter(const String& name, const String& value) override;
    };

    class FOW_ENGINE_API Text2DRendererComponent : public Component {
        TextSprite2DPtr m_pText;
        FontPtr m_pFont;
        String m_sText = "Sample Text";
        int m_iTextWrapWidth = 0;
        MaterialPtr m_pMaterial;
        IntRectangle m_TextRect = { 0, 0, 128, 128 };
    public:
        FOW_COMPONENT_CLASS(Text2DRendererComponent, Component)

        void on_spawn() override;
        void on_update(double dt) override;

        void set_material(const MaterialPtr& material);
        [[nodiscard]] FOW_CONSTEXPR const MaterialPtr& get_material() const { return m_pMaterial; }

        void set_font(const Font& font);
        void set_font(const FontPtr& font);
        [[nodiscard]] FOW_CONSTEXPR const FontPtr& get_font() const { return m_pFont; }

        void set_text(const String& text);
        [[nodiscard]] String get_text() const;

        void set_text_wrap_width(int width);
        [[nodiscard]] int get_text_wrap_width() const;

        void set_text_sprite(const TextSprite2DPtr& text);
        [[nodiscard]] FOW_CONSTEXPR const TextSprite2DPtr& get_text_sprite() const { return m_pText; }

        void set_text_rect(const IntRectangle& rect);
        [[nodiscard]] FOW_CONSTEXPR const IntRectangle& get_text_rect() const { return m_TextRect; }

        void set_parameter(const String& name, const String& value) override;
    };
}

FOW_REGISTER_COMPONENT(fow::TransformComponent,   "Transform");
FOW_REGISTER_COMPONENT(fow::Transform2DComponent, "Transform2D");
FOW_REGISTER_COMPONENT_WITH_DEPENDENCIES(fow::EnvironmentComponent,      "Environment",      /* Dependencies */  "Transform");
FOW_REGISTER_COMPONENT_WITH_DEPENDENCIES(fow::LightComponent,            "Light",            /* Dependencies */  "Transform");
FOW_REGISTER_COMPONENT_WITH_DEPENDENCIES(fow::CameraComponent,           "Camera",           /* Dependencies */  "Transform");
FOW_REGISTER_COMPONENT_WITH_DEPENDENCIES(fow::SpriteRendererComponent,   "SpriteRenderer",   /* Dependencies */  "Transform");
FOW_REGISTER_COMPONENT_WITH_DEPENDENCIES(fow::TextRendererComponent,     "TextRenderer",     /* Dependencies */  "Transform");
FOW_REGISTER_COMPONENT_WITH_DEPENDENCIES(fow::ModelRendererComponent,    "ModelRenderer",    /* Dependencies */  "Transform");
FOW_REGISTER_COMPONENT_WITH_DEPENDENCIES(fow::Sprite2DRendererComponent, "SpriteRenderer2D", /* Dependencies */  "Transform2D");
FOW_REGISTER_COMPONENT_WITH_DEPENDENCIES(fow::Text2DRendererComponent,   "TextRenderer2D",   /* Dependencies */  "Transform2D");

#endif