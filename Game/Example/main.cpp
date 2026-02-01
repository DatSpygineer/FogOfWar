#include "fow/Core.hpp"

#include "imgui.h"
#include "glm/gtx/matrix_interpolation.hpp"

using namespace fow;

struct LightInfo {
    glm::vec3 position;
    glm::vec3 color;

    float constant;
    float linear;
    float quadratic;
};

class Camera {
    glm::vec3 m_position, m_forward, m_right, m_up;
    float m_fYaw = 0.0f;
    float m_fPitch = 0.0f;
    bool m_bEnableFreeLook = false;
public:
    Camera() { }
    Camera(const glm::vec3& position) : m_position(position), m_forward(glm::vec3 { 0.0f, 0.0f, 1.0f }), m_right(glm::vec3 { 1.0f, 0.0f, 0.0f }), m_up(glm::vec3 { 0.0f, 1.0f, 0.0f }) {
        Renderer::UpdateCameraProjectionPerspective(60.0f, Engine::GetWindowSize(), 0.1f, 1000.0f);
        Renderer::UpdateCameraPosition(m_position, m_position + m_forward, m_up);
    }

    void update(const double dt) {
        auto update = false;

        if (Input::KeyIsPressed(KeyCode::Escape)) {
            m_bEnableFreeLook = !m_bEnableFreeLook;
            Input::SetCursorMode(m_bEnableFreeLook ? Input::CursorMode::Disabled : Input::CursorMode::Normal);
        }

        const auto x_axis = -Input::GetAxis("move_right", "move_left");
        const auto y_axis = Input::GetAxis("move_forward", "move_backward");
        const auto z_axis = Input::GetAxis("move_up", "move_down");
        const float speed = Input::KeyIsDown(KeyCode::LeftShift) ? 8.0f : 4.0f;
        auto motion = glm::vec3 { 0.0f };
        auto motion_y = 0.0f;
        if (x_axis != 0) {
            motion += m_right * x_axis * static_cast<float>(dt) * speed;
            update = true;
        }
        if (y_axis != 0) {
            motion += m_forward * y_axis * static_cast<float>(dt) * speed;
            update = true;
        }
        if (z_axis != 0) {
            motion_y += z_axis * static_cast<float>(dt) * speed;
            update = true;
        }
        const auto look_movement = Input::MouseMovement();
        if (look_movement.length() != 0.0f && m_bEnableFreeLook) {
            m_fPitch = static_cast<float>(m_fPitch + look_movement.y * dt * 2.0f);
            m_fYaw -= look_movement.x * dt * 2.0f;
            update = true;
        }

        if (update) {
            const auto forward = glm::normalize(glm::vec3((glm::axisAngleMatrix(m_up, m_fYaw) * glm::axisAngleMatrix(m_right, m_fPitch) * glm::vec4(m_forward, 0.0f))));
            m_position += glm::vec3((glm::axisAngleMatrix(m_up, m_fYaw) * glm::axisAngleMatrix(m_right, m_fPitch) * glm::vec4(motion, 0.0f))) + m_up * motion_y;
            Renderer::UpdateCameraPosition(m_position, m_position + forward, m_up);
        }
    }

    constexpr auto position() const {
        return m_position;
    }
};

class ExampleGame : public Game {
    Asset<Model> test_model;
    Asset<Model> test_plane_model;
    Asset<Material> skybox_material;
    float m_fAngle = 0.0f;

    float m_fPhongStrength  = 0.5f;
    float m_fPhongExponent  = 0.5f;
    float m_fEnvMapStrength = 0.5f;
    SkyboxPtr m_pSkybox = nullptr;

    Camera m_camera;

    LightInfo m_light1 = {
        .position  = glm::vec3 { 2.0f, 0.0f, 0.0f },
        .color     = glm::vec3 { 1.0f, 0.0f, 0.0f },
        .constant  = 1.0f,
        .linear    = 0.09f,
        .quadratic = 0.032f
    };
    LightInfo m_light2 = {
        .position  = glm::vec3 { -2.0f, 0.0f, 0.0f },
        .color     = glm::vec3 { 0.0f, 1.0f, 0.0f },
        .constant  = 1.0f,
        .linear    = 0.09f,
        .quadratic = 0.032f
    };

    float m_ambientLightStrength = 0.125f;
    glm::vec3 m_ambientLightColor = glm::vec3 { 1.0f, 1.0f, 1.0f };
    glm::vec3 m_sunDir   = glm::vec3 { 0.0f, -1.0f, 0.0f };
    glm::vec3 m_sunColor = glm::vec3 { 1.0 };
public:
    ExampleGame() : Game() { }

    void on_init() override {
        auto model = Assets::Load<Model>("/Models/Cube.model.xml");
        Debug::AssertFatal(model);
        if (model.has_value()) {
            test_model = std::move(model.value());
        }
        model = Assets::Load<Model>("/Models/Plane.model.xml");
        Debug::AssertFatal(model);
        if (model.has_value()) {
            test_plane_model = std::move(model.value());
        }

        auto material = Assets::Load<Material>("/Materials/SkyTest.material.xml");
        Debug::AssertFatal(material);
        if (material.has_value()) {
            skybox_material = material.value();
        }

        Input::CreateAction("move_left",     Input::Type::KeyboardKey, KeyCode::A);
        Input::CreateAction("move_right",    Input::Type::KeyboardKey, KeyCode::D);
        Input::CreateAction("move_forward",  Input::Type::KeyboardKey, KeyCode::W);
        Input::CreateAction("move_backward", Input::Type::KeyboardKey, KeyCode::S);
        Input::CreateAction("move_up",       Input::Type::KeyboardKey, KeyCode::Space);
        Input::CreateAction("move_down",     Input::Type::KeyboardKey, KeyCode::LeftControl);

        m_camera = Camera { glm::vec3 { 0.0f, 0.0f, -5.0f } };
        m_pSkybox = std::make_shared<Skybox>(skybox_material.ptr());
    }
    void on_update(const double dt) override {
        m_camera.update(dt);
        m_fAngle += static_cast<float>(dt * 0.5f);
        if (Input::KeyIsPressed(KeyCode::F10)) {
            Console::ToggleConsoleVisible();
        }
    }
    void on_render(const double dt) override {
        m_pSkybox->draw();
        for (const auto& mat : test_model->materials()) {
            Debug::Assert(mat->set_parameter("ViewPos", m_camera.position()));
            Debug::Assert(mat->set_parameter("PhongStrength", m_fPhongStrength));
            Debug::Assert(mat->set_parameter("PhongExponent", m_fPhongExponent));
            Debug::Assert(mat->set_parameter("Environment.AmbientColor", m_ambientLightColor));
            Debug::Assert(mat->set_parameter("Environment.AmbientStrength", m_ambientLightStrength));
            Debug::Assert(mat->set_parameter("Environment.SunDirection", m_sunDir));
            Debug::Assert(mat->set_parameter("Environment.SunLightColor", m_sunColor));
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
        test_model->draw();
        test_model->draw(Transform { glm::vec3 { 4.0f, 0.0f, -4.0f }, glm::vec3 { 1.0f }, glm::quat { } });
        test_model->draw(Transform { glm::vec3 { -4.0f, 0.0f, 4.0f }, glm::vec3 { 1.0f }, glm::quat { } });
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
            ImGui::InputFloat3("Sun Direction", glm::value_ptr(m_sunDir));
            ImGui::ColorEdit3("Sun Color", glm::value_ptr(m_sunColor));
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
