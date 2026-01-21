#include <fow/Core.hpp>

#include "imgui.h"

using namespace fow;

class ExampleGame : public Game {
    Asset<Texture2D> test_texture;
    Asset<Shader> test_shader;
    Asset<Material> test_material;
    Asset<Model> test_model;
    Transform test_transform;
    float m_fAngle = 0.0f;
public:
    ExampleGame() : Game() { }

    void on_init() override {
        auto texture = Assets::Load<Texture2D>("/Textures/test.texture.xml");
        Debug::AssertFatal(texture);
        if (texture.has_value()) {
            test_texture = std::move(texture.value());
        }

        auto shader = Assets::Load<Shader>("/Shaders/test.shader.xml");
        Debug::AssertFatal(shader);
        if (shader.has_value()) {
            test_shader = std::move(shader.value());
        }

        auto material = Assets::Load<Material>("/Materials/test.material.xml");
        Debug::AssertFatal(material);
        if (material.has_value()) {
            test_material = std::move(material.value());
        }

        auto model = Assets::Load<Model>("/Models/Duck.model.xml");
        Debug::AssertFatal(model);
        if (model.has_value()) {
            test_model = std::move(model.value());
        }

        Renderer::UpdateCameraProjectionPerspective(60.0f, Engine::GetWindowSize(), 0.1f, 1000.0f);
        Renderer::UpdateCameraPosition(glm::vec3 { 0.0f, 50.0f, 100.0f }, glm::vec3 { 0.0f, 40.0f, 0.0f }, glm::vec3 { 0.0f, 1.0f, 0.0f });
        test_transform.set_local_scale(0.35f);
    }
    void on_update(const double dt) override {
        test_transform.set_local_rotation(glm::vec3 { 0.0f, 1.0f, 0.0f }, m_fAngle);
        m_fAngle += static_cast<float>(dt);
    }
    void on_render(const double dt) override {
        test_model->draw(test_transform);
    }
    void on_close() override {
    }
    void on_window_resized(const glm::ivec2& new_size) override {
    }

    [[nodiscard]] constexpr String title() const override {
        return "Example";
    }
    [[nodiscard]] Vector<String> game_data_archives() const override {
        return Vector<String> { "Data.fwpak" };
    }
    [[nodiscard]] Path base_data_path() const override {
        return Engine::GetGameBasePath() / "data";
    }
    [[nodiscard]] Option<Path> mod_data_path() const override {
        return Engine::GetGameBasePath() / "mods";
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
