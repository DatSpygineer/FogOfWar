#pragma once

#include "fow/Core.hpp"
#include "Components.hpp"

using namespace fow;

class Scene3DExample {
    SkyboxPtr m_pSkybox;
    ScenePtr m_pScene;

    ComponentPtr<TransformComponent> m_pLight1Transform = nullptr;
    ComponentPtr<LightComponent> m_pLight1 = nullptr;
    ComponentPtr<TransformComponent> m_pLight2Transform = nullptr;
    ComponentPtr<LightComponent> m_pLight2 = nullptr;
public:
    Scene3DExample() {
        auto model = Assets::Load<Model>("/Models/Sphere.model.xml");
        Debug::AssertFatal(model);
        if (!model.has_value()) {
            return;
        }

        auto light_model = Assets::Load<Model>("/Models/Light.model.xml");
        Debug::AssertFatal(light_model);
        if (!light_model.has_value()) {
            return;
        }

        auto light_sprite = Assets::Load<Sprite>("/Sprites/Light.sprite.xml");
        Debug::AssertFatal(light_sprite);
        if (!light_sprite.has_value()) {
            return;
        }

        auto material = Assets::Load<Material>("/Materials/SkyTest.material.xml");
        Debug::AssertFatal(material);
        if (material.has_value()) {
            m_pSkybox = CreateRef<Skybox>(std::move(material.value().ptr()));
        }

        m_pScene = CreateRef<Scene>();
        const auto ent_camera = m_pScene->create_entity();
        const auto comp_camera_transform = ent_camera->add_component<TransformComponent>();
        comp_camera_transform->set_position(Vector3Constants::Forward * -2.5f);
        ent_camera->add_component<FlyCameraComponent>();

        const auto ent_model = m_pScene->create_entity();
        const auto comp_model_transform = ent_model->add_component<TransformComponent>();
        comp_model_transform->set_position(Vector3Constants::Forward * 2.5f);
        const auto comp_model = ent_model->add_component<ModelRendererComponent>();
        comp_model->set_model(model.value().ptr());
        ent_model->add_component<TestSphereComponent>();

        const auto ent_light_1 = m_pScene->create_entity();
        m_pLight1Transform = ent_light_1->add_component<TransformComponent>();
        m_pLight1Transform->set_position(Vector3 { -5.0f, 1.0f, 5.0f });
        m_pLight1 = ent_light_1->add_component<LightComponent>();
        m_pLight1->set_color(Color { 1.0f, 1.0f, 1.0f });
        m_pLight1->set_intensity(300.0f);
        const auto comp_light_1_spr = ent_light_1->add_component<SpriteRendererComponent>();
        comp_light_1_spr->set_sprite(light_sprite.value().ptr());

        const auto ent_light_2 = m_pScene->create_entity();
        m_pLight2Transform = ent_light_2->add_component<TransformComponent>();
        m_pLight2Transform->set_position(Vector3 { 5.0f, 1.0f, -5.0f });
        m_pLight2 = ent_light_2->add_component<LightComponent>();
        m_pLight2->set_color(Color { 1.0f, 0.0f, 1.0f });
        m_pLight2->set_intensity(300.0f);
        const auto comp_light_2_spr = ent_light_2->add_component<SpriteRendererComponent>();
        comp_light_2_spr->set_sprite(light_sprite.value().ptr());

        const auto ent_env = m_pScene->create_entity();
        const auto comp_env_transform = ent_env->add_component<TransformComponent>();
        comp_env_transform->set_rotation_deg(Vector3Constants::UnitX * 60.0f);
        const auto comp_env = ent_env->add_component<EnvironmentComponent>();
        comp_env->set_sunlight_color(Color::FromInt(0xE64120FF));
        comp_env->set_sunlight_intensity(300.0f);
        comp_env->set_skybox(m_pSkybox);

        Engine::SetScene(m_pScene);
    }
};

class Scene2DExample {
    ScenePtr m_pScene;
public:
    Scene2DExample() {
        m_pScene = CreateRef<Scene>();

        const auto light_sprite = Assets::Load<Sprite2D>("/Sprites/Light.sprite.xml");
        Debug::AssertFatal(light_sprite);
        if (!light_sprite.has_value()) {
            return;
        }


    }
};