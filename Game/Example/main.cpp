#include <fow/Core.hpp>

#include <fow/Engine/ImGui.hpp>

using namespace fow;

struct LightInfo {
    glm::vec3 position;
    glm::vec3 color;

    float constant;
    float linear;
    float quadratic;
};

class ExampleGame : public Game {
    Asset<Model> test_model;
    Asset<Model> light_model;
    Transform test_transform;
    float m_fAngle = 0.0f;
    glm::vec3 m_cameraPos = glm::vec3 { 0.0f, 5.0f, 5.0f };

    float m_fPhongStrength  = 0.5f;
    float m_fPhongExponent  = 0.5f;
    float m_fEnvMapStrength = 0.5f;

    LightInfo m_light1 = {
        .position  = glm::vec3 { 2.0f, 0.0f, 0.0f },
        .color     = glm::vec3 { 1.0f, 0.0f, 0.0f },
        .constant  = 1.0f,
        .linear    = 0.0f,
        .quadratic = 0.0f
    };
    LightInfo m_light2 = {
        .position  = glm::vec3 { -2.0f, 0.0f, 0.0f },
        .color     = glm::vec3 { 0.0f, 1.0f, 0.0f },
        .constant  = 1.0f,
        .linear    = 0.0f,
        .quadratic = 0.0f
    };

    float m_ambientLightStrength = 0.125f;
    glm::vec3 m_ambientLightColor = glm::vec3 { 1.0f, 1.0f, 1.0f };
public:
    ExampleGame() : Game() { }

    void on_init() override {
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
            Debug::Assert(mat->set_parameter("Environment.AmbientColor", m_ambientLightColor));
            Debug::Assert(mat->set_parameter("Environment.AmbientStrength", m_ambientLightStrength));
            Debug::Assert(mat->set_parameter("Lights[0].Position", m_light1.position));
            Debug::Assert(mat->set_parameter("Lights[0].Color", m_light1.color));
            Debug::Assert(mat->set_parameter("Lights[0].Constant", m_light1.constant));
            Debug::Assert(mat->set_parameter("Lights[0].Linear", m_light1.linear));
            Debug::Assert(mat->set_parameter("Lights[0].Quadratic", m_light1.quadratic));
            Debug::Assert(mat->set_parameter("Lights[1].Position", m_light2.position));
            Debug::Assert(mat->set_parameter("Lights[1].Color", m_light2.color));
            Debug::Assert(mat->set_parameter("Lights[1].Constant", m_light2.constant));
            Debug::Assert(mat->set_parameter("Lights[1].Linear", m_light2.linear));
            Debug::Assert(mat->set_parameter("Lights[1].Quadratic", m_light2.quadratic));
            Debug::Assert(mat->set_parameter("LightCount", 2));
            Debug::Assert(mat->set_parameter("EnvMapStrength", m_fEnvMapStrength));
        }
        test_model->draw(test_transform);

        for (auto& mat : light_model->materials()) {
            Debug::AssertWarn(mat->set_parameter("TextureColor", glm::vec4(m_light1.color, 1.0f)));
        }
        light_model->draw(Transform { m_light1.position, glm::vec3 { 1.0f }, glm::quat() });
        for (auto& mat : light_model->materials()) {
            Debug::AssertWarn(mat->set_parameter("TextureColor", glm::vec4(m_light2.color, 1.0f)));
        }
        light_model->draw(Transform { m_light2.position, glm::vec3 { 1.0f }, glm::quat() });
    }
    void on_close() override {
    }
    void on_window_resized(const glm::ivec2& new_size) override {
    }

#if __cplusplus >= 202302L
    [[nodiscard]] constexpr String title() const override {
#else
    [[nodiscard]] inline String title() const override {
#endif
        return "Example";
    }
    [[nodiscard]] Vector<String> game_data_archives() const override {
        return Vector<String> { "Data.pak" };
    }
    [[nodiscard]] Path base_data_path() const override {
        return Engine::GetGameBasePath() / "data";
    }
    [[nodiscard]] Option<Path> mod_data_path() const override {
        return Engine::GetGameBasePath() / "mods";
    }
    void on_update_imgui(double dt) override {
        ImGui::Begin("Shader test");
        ImGui::SeparatorText("Environment Parameters");
            ImGui::SliderFloat("Ambient Strength", &m_ambientLightStrength, 0.0f, 1.0f);
            ImGui::ColorEdit3("Ambient Color", glm::value_ptr(m_ambientLightColor));
        ImGui::SeparatorText("Phong Parameters");
            ImGui::SliderFloat("Phong Strength", &m_fPhongStrength, 0.0f, 1.0f);
            ImGui::SliderFloat("Phong Exponent", &m_fPhongExponent, 0.0f, 1.0f);
            ImGui::SliderFloat("Envmap Strength", &m_fEnvMapStrength, 0.0f, 1.0f);
        ImGui::SeparatorText("Light 1 Parameters");
            ImGui::InputFloat3("L1 Position", glm::value_ptr(m_light1.position));
            ImGui::ColorEdit3("L1 Color", glm::value_ptr(m_light1.color), ImGuiColorEditFlags_Float);
            ImGui::InputFloat("L1 Linear", &m_light1.linear);
            ImGui::InputFloat("L1 Constant", &m_light1.constant);
            ImGui::InputFloat("L1 Quadratic", &m_light1.quadratic);
        ImGui::SeparatorText("Light 2 Parameters");
            ImGui::InputFloat3("L2 Position", glm::value_ptr(m_light2.position));
            ImGui::ColorEdit3("L2 Color", glm::value_ptr(m_light2.color), ImGuiColorEditFlags_Float);
            ImGui::InputFloat("L2 Linear", &m_light2.linear);
            ImGui::InputFloat("L2 Constant", &m_light2.constant);
            ImGui::InputFloat("L2 Quadratic", &m_light2.quadratic);
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
