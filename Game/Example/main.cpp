#include "fow/Core.hpp"

#include "imgui.h"
#include "fow/Engine/Entity.hpp"

#include "FlyCameraComponent.hpp"

using namespace fow;

class ExampleGame : public Game {
    Asset<Material> n_skyboxMaterial;
    ScenePtr m_pScene;
public:
    ExampleGame() : Game() { }

    void on_init() override {
        auto model = Assets::Load<Model>("/Models/Sphere.model.xml");
        Debug::AssertFatal(model);
        if (!model.has_value()) {
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
        ent_camera->enable();
        ent_camera->add_component<FlyCameraComponent>();

        const auto ent_model = m_pScene->create_entity();
        ent_model->enable();
        const auto comp_model = ent_model->add_component<ModelRendererComponent>();
        comp_model->set_model(model.value().ptr());

        const auto ent_light_1 = m_pScene->create_entity();
        ent_light_1->enable();
        const auto comp_light_1_transform = ent_light_1->add_component<TransformComponent>();
        comp_light_1_transform->transform().set_position(Vector3 { -10.0f, -5.0f, 0.0f });
        const auto comp_light_1 = ent_light_1->add_component<LightComponent>();
        comp_light_1->set_color(Color { 1.0f, 1.0f, 1.0f });
        comp_light_1->set_intensity(300.0f);

        const auto ent_light_2 = m_pScene->create_entity();
        ent_light_2->enable();
        const auto comp_light_2_transform = ent_light_2->add_component<TransformComponent>();
        comp_light_2_transform->transform().set_position(Vector3 { 10.0f, 5.0f, 0.0f });
        const auto comp_light_2 = ent_light_1->add_component<LightComponent>();
        comp_light_2->set_color(Color { 1.0f, 1.0f, 1.0f });
        comp_light_2->set_intensity(300.0f);

        const auto ent_env = m_pScene->create_entity();
        ent_env->enable();
        const auto comp_env_transform = ent_light_2->add_component<TransformComponent>();
        comp_env_transform->transform().set_rotation_deg(Vector3Constants::UnitX * 180.0f);
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

int main(const int argc, char** argv) {
    Debug::AssertFatal(Engine::Initialize(argc, argv, "Example", []() -> std::shared_ptr<Game> {
        return std::make_shared<ExampleGame>();
    }));
    Engine::SetBackgroundColor(Color { 0.25f, 0.5f, 1.0f });
    Engine::Run();
    return 0;
}
