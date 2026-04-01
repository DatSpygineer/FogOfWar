#include "fow/Core.hpp"

#include "imgui.h"
#include "fow/Engine/Entity.hpp"

#include "Components.hpp"

using namespace fow;

class ExampleGame : public Game {
    Asset<Material> n_skyboxMaterial;
    ScenePtr m_pScene;

    ComponentPtr<TransformComponent> m_pLight1Transform = nullptr;
    ComponentPtr<LightComponent> m_pLight1 = nullptr;
    ComponentPtr<TransformComponent> m_pLight2Transform = nullptr;
    ComponentPtr<LightComponent> m_pLight2 = nullptr;
public:
    ExampleGame() : Game() { }

    void on_init() override {
        auto model = Assets::Load<Model>("/Models/Cube.model.xml");
        Debug::AssertFatal(model);
        if (!model.has_value()) {
            return;
        }

        auto light_model = Assets::Load<Model>("/Models/Light.model.xml");
        Debug::AssertFatal(light_model);
        if (!light_model.has_value()) {
            return;
        }

        auto material = Assets::Load<Material>("/Materials/SkyTest.material.xml");
        Debug::AssertFatal(material);
        if (material.has_value()) {
            n_skyboxMaterial = material.value();
        }

        Input::CreateAction("move_left",     Input::Type::KeyboardKey, KeyCode::A);
        Input::CreateAction("move_right",    Input::Type::KeyboardKey, KeyCode::D);
        Input::CreateAction("move_forward",  Input::Type::KeyboardKey, KeyCode::W);
        Input::CreateAction("move_backward", Input::Type::KeyboardKey, KeyCode::S);
        Input::CreateAction("move_up",       Input::Type::KeyboardKey, KeyCode::Space);
        Input::CreateAction("move_down",     Input::Type::KeyboardKey, KeyCode::LeftControl);

        RenderQueue::SetSkybox(std::make_shared<Skybox>(n_skyboxMaterial.ptr()));

        m_pScene = std::make_shared<Scene>();
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
        const auto comp_light_1_model = ent_light_1->add_component<ModelRendererComponent>();
        comp_light_1_model->set_model(light_model.value().ptr());

        const auto ent_light_2 = m_pScene->create_entity();
        m_pLight2Transform = ent_light_2->add_component<TransformComponent>();
        m_pLight2Transform->set_position(Vector3 { 5.0f, 1.0f, -5.0f });
        m_pLight2 = ent_light_2->add_component<LightComponent>();
        m_pLight2->set_color(Color { 1.0f, 1.0f, 1.0f });
        m_pLight2->set_intensity(300.0f);
        const auto comp_light_2_model = ent_light_2->add_component<ModelRendererComponent>();
        comp_light_2_model->set_model(light_model.value().ptr());

        const auto ent_env = m_pScene->create_entity();
        const auto comp_env_transform = ent_light_2->add_component<TransformComponent>();
        comp_env_transform->set_rotation_deg(Vector3Constants::UnitX * 180.0f);
        const auto comp_env = ent_env->add_component<EnvironmentComponent>();
        comp_env->set_sunlight_color(Color { 1.0f, 1.0f, 1.0f });
        comp_env->set_sunlight_intensity(100.0f);
        comp_env->set_skybox(std::make_shared<Skybox>(n_skyboxMaterial.ptr()));

        Engine::SetScene(m_pScene);
    }
    void on_update(const double dt) override {
    }
    void on_render(const double dt) override {
    }
    void on_close() override {
    }

    [[nodiscard]] FOW_CONSTEXPR String title() const override {
        return "Example";
    }
    [[nodiscard]] Vector<String> game_data_archives() const override {
        return Vector<String> { "Data.pak" };
    }
    [[nodiscard]] bool allow_mods() const override {
        return FOW_MODS_ENABLED;
    }
    void on_update_imgui(double dt) override {
    }
};

FOW_ENTRY_POINT(ExampleGame)