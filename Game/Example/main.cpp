#include <fow/Core.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "imgui.h"

using namespace fow;

class ExampleGame : public Game {
    Asset<Texture2D> test_texture;
    Asset<Texture2D> test_texture_s;
    Asset<Shader> test_shader;
    Asset<Material> test_material;
    Asset<Model> test_model;
    Asset<Model> light_model;
    Transform test_transform;
    float m_fAngle = 0.0f;
    glm::vec3 m_cameraPos = glm::vec3 { 0.0f, 5.0f, 5.0f };

    float m_fPhongStrength = 1.0f;
    float m_fPhongExponent = 1.0f;
    glm::vec3 m_lightPos    = glm::vec3 { 4.0f, 0.0f, 0.0f };
    glm::vec3 m_lightColor  = glm::vec3 { 1.0f, 0.0f, 0.0f };
    glm::vec3 m_lightPos2   = glm::vec3 { -4.0f, 0.0f, 0.0f };
    glm::vec3 m_lightColor2 = glm::vec3 { 0.0f, 1.0f, 0.0f };
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

        auto model = Assets::Load<Model>("/Models/Cube.model.xml");
        Debug::AssertFatal(model);
        if (model.has_value()) {
            test_model = std::move(model.value());
        }
        model = Assets::Load<Model>("/Models/LightGizmo.model.xml");
        Debug::AssertFatal(model);
        if (model.has_value()) {
            light_model = std::move(model.value());
        }

        Renderer::UpdateCameraProjectionPerspective(60.0f, Engine::GetWindowSize(), 0.1f, 1000.0f);
        Renderer::UpdateCameraPosition(m_cameraPos, glm::vec3 { 0.0f, 0.0f, 0.0f }, glm::vec3 { 0.0f, 1.0f, 0.0f });
    }
    void on_update(const double dt) override {
        test_transform.set_local_rotation(glm::vec3 { 0.0f, 1.0f, 0.0f }, m_fAngle);
        m_fAngle += static_cast<float>(dt * 0.5f);
    }
    void on_render(const double dt) override {
        for (auto& mat : test_model->materials()) {
            Debug::Assert(mat->set_parameter("ViewPos", m_cameraPos));
            Debug::Assert(mat->set_parameter("PhongStrength", m_fPhongStrength));
            Debug::Assert(mat->set_parameter("PhongExponent", m_fPhongExponent));
            Debug::Assert(mat->set_parameter("Lights[0].Position", m_lightPos));
            Debug::Assert(mat->set_parameter("Lights[0].Color", m_lightColor));
            Debug::Assert(mat->set_parameter("Lights[1].Position", m_lightPos2));
            Debug::Assert(mat->set_parameter("Lights[1].Color", m_lightColor2));
            Debug::Assert(mat->set_parameter("LightCount", 2));
        }
        test_model->draw(test_transform);

        for (auto& mat : light_model->materials()) {
            mat->set_parameter("TextureColor", glm::vec4(m_lightColor, 1.0f));
        }
        light_model->draw(Transform { m_lightPos, glm::vec3 { 1.0f }, glm::quat() });
        for (auto& mat : light_model->materials()) {
            mat->set_parameter("TextureColor", glm::vec4(m_lightColor2, 1.0f));
        }
        light_model->draw(Transform { m_lightPos2, glm::vec3 { 1.0f }, glm::quat() });
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
        ImGui::Begin("Shader test");
            ImGui::SliderFloat("Phong Strength", &m_fPhongStrength, 0.0f, 1.0f);
            ImGui::SliderFloat("Phong Exponent", &m_fPhongExponent, 0.0f, 255.0f);
        ImGui::Separator();
            ImGui::InputFloat3("Light 1 position", glm::value_ptr(m_lightPos));
            ImGui::ColorEdit3("Light 1 color", glm::value_ptr(m_lightColor), ImGuiColorEditFlags_Float);
        ImGui::Separator();
            ImGui::InputFloat3("Light 2 position", glm::value_ptr(m_lightPos2));
            ImGui::ColorEdit3("Light 2 color", glm::value_ptr(m_lightColor2), ImGuiColorEditFlags_Float);
        ImGui::End();
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
