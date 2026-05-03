#pragma once

#include "fow/Core.hpp"
#include "Components.hpp"
#include "imgui.h"

using namespace fow;

class SceneExample {
    SkyboxPtr m_pSkybox;
    ScenePtr m_pScene;
public:
    SceneExample() {
        auto sphere_model = Assets::Load<Model>("/Models/Sphere.model.xml");
        Debug::AssertFatal(sphere_model);
        if (!sphere_model.has_value()) {
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

        auto sky_material = Assets::Load<Material>("/Materials/SkyTest.material.xml");
        Debug::AssertFatal(sky_material);
        if (sky_material.has_value()) {
            m_pSkybox = CreateRef<Skybox>(std::move(sky_material.value().ptr()));
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
        comp_model->set_model(sphere_model.value().ptr());
        ent_model->add_component<TestSphereComponent>();

        const auto ent_text = m_pScene->create_entity();
        const auto comp_text_transform = ent_text->add_component<TransformComponent>();
        const auto comp_text = ent_text->add_component<TextRendererComponent>();
        comp_text->set_font(CreateRef<Font>("Roboto-Regular.ttf", 42));
        comp_text->set_text("Hello World!");
        comp_text->set_text_rect(IntRectangle { 0, 0, 512, 512 });
        comp_text->set_billboard_mode(BillboardMode::BillboardSpherical);

        const auto ent_light_1 = m_pScene->create_entity();
        auto comp_light_1_transform = ent_light_1->add_component<TransformComponent>();
        comp_light_1_transform->set_position(Vector3 { -5.0f, 1.0f, 5.0f });
        auto comp_light_1 = ent_light_1->add_component<LightComponent>();
        comp_light_1->set_color(Color { 1.0f, 1.0f, 1.0f });
        comp_light_1->set_intensity(300.0f);
        const auto comp_light_1_spr = ent_light_1->add_component<SpriteRendererComponent>();
        comp_light_1_spr->set_sprite(light_sprite.value().ptr());

        const auto ent_light_2 = m_pScene->create_entity();
        auto comp_light_2_transform = ent_light_2->add_component<TransformComponent>();
        comp_light_2_transform->set_position(Vector3 { 5.0f, 1.0f, -5.0f });
        auto comp_light_2 = ent_light_2->add_component<LightComponent>();
        comp_light_2->set_color(Color { 1.0f, 0.0f, 1.0f });
        comp_light_2->set_intensity(300.0f);
        const auto comp_light_2_spr = ent_light_2->add_component<SpriteRendererComponent>();
        comp_light_2_spr->set_sprite(light_sprite.value().ptr());

        const auto ent_env = m_pScene->create_entity();
        const auto comp_env_transform = ent_env->add_component<TransformComponent>();
        comp_env_transform->set_rotation_deg(Vector3Constants::UnitX * 60.0f);
        const auto comp_env = ent_env->add_component<EnvironmentComponent>();
        comp_env->set_sunlight_color(Color::FromInt(0xE64120FF));
        comp_env->set_sunlight_intensity(300.0f);
        comp_env->set_skybox(m_pSkybox);

        auto ui = m_pScene->ui_frame();
        auto checkbox = CreateRef<UI::CheckBox>(ui);
        checkbox->set_area({ 64, 64, 64, 64 });
        ui->add_widget(checkbox);

        auto button = CreateRef<UI::Button>(ui, "Example");
        button->set_area({ 128 + 5, 64, 128, 64 });
        ui->add_widget(button);

        auto panel = CreateRef<UI::Panel>(ui);
        panel->set_area({ 512, 512, 64, 64 });
        ui->add_widget(panel);

        Engine::SetScene(m_pScene);
    }
};