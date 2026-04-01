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

        const Transform& get_parent() const;

        void set_transform(const Transform& transform);

        void set_parameter(const String& name, const String& value) override;
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
        Color m_color;
        float m_intensity;
        LightInfoPtr m_pLightInfo;
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
}

FOW_REGISTER_COMPONENT(fow::TransformComponent, "Transform");
FOW_REGISTER_COMPONENT_WITH_DEPENDENCIES(fow::EnvironmentComponent,   "Environment",   /* Dependencies */  "Transform");
FOW_REGISTER_COMPONENT_WITH_DEPENDENCIES(fow::LightComponent,         "Light",         /* Dependencies */  "Transform");
FOW_REGISTER_COMPONENT_WITH_DEPENDENCIES(fow::CameraComponent,        "Camera",        /* Dependencies */  "Transform");
FOW_REGISTER_COMPONENT_WITH_DEPENDENCIES(fow::ModelRendererComponent, "ModelRenderer", /* Dependencies */  "Transform");

#endif