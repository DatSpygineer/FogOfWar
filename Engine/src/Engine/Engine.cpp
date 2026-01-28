#include "fow/Engine.hpp"
#include "fow/Engine/Convar.hpp"

#include "fow/Renderer.hpp"
#include <GLFW/glfw3.h>

#include "SOIL2.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

namespace fow {
    static void UpdateResolution(const CVarPtr& self);
    static void UpdateWindowMode(const CVarPtr& self);
    static void UpdateMonitorIndex(const CVarPtr& self);
    static void UpdateVSync(const CVarPtr& self);
    static void UpdateLanguage(const CVarPtr& self);
    static Result<> QuitCommand(const Vector<String>& args);

    const auto vid_resolution  = CVar::Create("vid_resolution",  glm::vec2(1280, 720), CVarFlags::UserSettings | CVarFlags::SaveToConfig, &UpdateResolution);
    const auto vid_window_mode = CVar::Create("vid_window_mode", "Windowed",           CVarFlags::UserSettings | CVarFlags::SaveToConfig, &UpdateWindowMode);
    const auto vid_monitor_idx = CVar::Create("vid_monitor_idx", 0,                    CVarFlags::UserSettings | CVarFlags::SaveToConfig, &UpdateMonitorIndex);
    const auto vid_vsync       = CVar::Create("vid_vsync",       false,                CVarFlags::UserSettings | CVarFlags::SaveToConfig, &UpdateVSync);
    const auto cl_lang         = CVar::Create("cl_lang",         "en_us",              CVarFlags::UserSettings | CVarFlags::SaveToConfig, &UpdateLanguage);
    const auto quit            = CVar::Create("quit",            QuitCommand,          CVarFlags::Default);

    namespace Engine {
        static bool s_initialized = false;
        static GLFWwindow* s_window = nullptr;
        static String s_window_title = "FogOfWar";
        
        static Color s_background_color = { 0.25f, 0.25f, 0.25f };
        static SharedPtr<Game> s_game_class = nullptr;
        static Path s_base_path = Path::CurrentDir();
        static const auto s_version = Version {
            0, 1, 0
        };

        Path GetGameBasePath() {
            return s_base_path;
        }

        Path GetResourcesPath() {
            return s_base_path / "res";
        }

        void SetBackgroundColor(const Color& color) {
            s_background_color = color;
        }
        void SetBackgroundColor(Color&& color) noexcept {
            s_background_color = color;
        }
        Color GetBackgroundColor() {
            return s_background_color;
        }

        Result<> Initialize(int argc, char** argv, const String& title, const std::function<std::shared_ptr<Game>()>& game_class_ctor) {
            if (s_initialized) {
                return Failure("Engine is already initialized!");
            }

            s_base_path = Path(argv[0]).parent();
            Debug::Initialize(s_base_path / "logs");
            s_game_class = std::move(game_class_ctor());

            Assets::Initialize(s_game_class->base_data_path(), s_game_class->game_data_archives(), s_game_class->mod_data_path());
            Console::Initialize();

            HashMap<String, Vector<String>> args;
            String current_arg = "";
            for (int i = 1; i < argc; ++i) {
                if (argv[i][0] == '-') {
                    current_arg = argv[i];
                    args.emplace(current_arg, Vector<String> { });
                } else if (!current_arg.is_empty()) {
                    args.at(current_arg).emplace_back(argv[i]);
                }
            }
            LoadLanguageFiles();

            if (glfwInit() != GLFW_TRUE) {
                const char* message;
                int code = glfwGetError(&message);
                return Failure(std::format("Failed to initialize GLFW: \"{}\" (code {})", message, code));
            }

            glm::vec2 resolution   = vid_resolution->as_vec2().value_or(glm::vec2 { 1280, 720 });
            WindowMode window_mode = rfl::string_to_enum<WindowMode>(vid_window_mode->as_string().value_or("Windowed").as_std_str())
                                        .value_or(WindowMode::Windowed);
            bool vsync_enabled     = vid_vsync->as_bool().value_or(false);
            int monitor_index      = vid_monitor_idx->as_int().value_or(0);

            if (args.contains("-resolution")) {
                if (const auto vals = args.at("-resolution"); vals.size() == 2) {
                    auto w = StringToInt<int>(vals.at(0));
                    auto h = StringToInt<int>(vals.at(1));
                    if (!w.has_value()) {
                        Debug::LogError("Failed to parse argument \"-resolution\": Width has invalid format!");
                    } else if (!h.has_value()) {
                        Debug::LogError("Failed to parse argument \"-resolution\": Height has invalid format!");
                    } else {
                        resolution = glm::ivec2(w.value(), h.value());
                    }
                } else {
                    Debug::LogError("Failed to parse argument \"-resolution\": Expected 2 values!");
                }
            }
            if (args.contains("-fullscreen")) {
                if (const auto vals = args.at("-fullscreen"); vals.size() >= 1) {
                    if (const auto fs = StringToBool(vals.at(0)); fs.has_value()) {
                        window_mode = fs.value() ? WindowMode::Fullscreen : WindowMode::Windowed;
                    } else {
                        Debug::LogError("Failed to parse argument \"-fullscreen\": Invalid format!");
                    }
                } else {
                    window_mode = WindowMode::Fullscreen;
                }
            }
            if (args.contains("-windowed")) {
                if (const auto vals = args.at("-windowed"); vals.size() >= 1) {
                    if (const auto win = StringToBool(vals.at(0)); win.has_value()) {
                        window_mode = win.value() ? WindowMode::Windowed : WindowMode::Fullscreen;
                    } else {
                        Debug::LogError("Failed to parse argument \"-windowed\": Invalid format!");
                    }
                } else {
                    window_mode = WindowMode::Windowed;
                }
            }
            if (args.contains("-vsync")) {
                if (const auto vals = args.at("-vsync"); vals.size() >= 1) {
                    if (const auto vsync = StringToBool(vals.at(0)); vsync.has_value()) {
                        vsync_enabled = vsync.value();
                    } else {
                        Debug::LogError("Failed to parse argument \"-vsync\": Invalid format!");
                    }
                } else {
                    vsync_enabled = true;
                }
            }
            if (args.contains("-monitor")) {
                if (const auto vals = args.at("-monitor"); vals.size() >= 1) {
                    if (const auto monitor_index_result = StringToInt<int>(vals.at(0)); monitor_index_result.has_value()) {
                        monitor_index = monitor_index_result.value();
                    } else {
                        Debug::LogError("Failed to parse argument \"-monitor\": Invalid format!");
                    }
                } else {
                    Debug::LogError("Failed to parse argument \"-monitor\": Expected an integer value!");
                }
            }
            if (args.contains("-dev")) {
                Console::SetConsoleVisible(true);
            }

            GLFWmonitor* monitor = nullptr;
            if (window_mode == WindowMode::Fullscreen) {
                if (monitor_index < 0) {
                    monitor = glfwGetPrimaryMonitor();
                } else {
                    int monitor_count = 0;
                    GLFWmonitor** monitors = glfwGetMonitors(&monitor_count);
                    if (monitor_index < monitor_count) {
                        monitor = monitors[monitor_index];
                    } else {
                        Debug::LogWarning(std::format("Minitor index {} is out of range! Defaulting to primary monitor!", monitor_index));
                        monitor = glfwGetPrimaryMonitor();
                        monitor_index = -1;
                    }
                }
            }

            s_window = glfwCreateWindow(resolution.x, resolution.y, title.as_cstr(), monitor, nullptr);

            if (s_window == nullptr) {
                const char* message;
                int code = glfwGetError(&message);
                glfwTerminate();
                return Failure(std::format("Failed to create window: \"{}\" (code {})", message, code));
            }

            s_window_title = title;
            glfwMakeContextCurrent(s_window);

            if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
                const auto error = glGetError();
                glfwDestroyWindow(s_window);
                glfwTerminate();
                return Failure(std::format("Failed to initialize OpenGL: GL error {}", error));
            }

            Debug::AssertFatal(Renderer::Initialize(s_base_path));
            Renderer::EnableBlend(true);
            Renderer::SetViewport(0.0f, 0.0f, resolution.x, resolution.y);
            glfwSetWindowSizeCallback(s_window, [](GLFWwindow* window, const int width, const int height) {
                DISCARD(window);
                Renderer::SetViewport(0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height));
                if (s_game_class != nullptr) {
                    s_game_class->on_window_resized(glm::ivec2 { width, height });
                }
            });

            glfwSwapInterval(vsync_enabled ? 1 : 0);

            IMGUI_CHECKVERSION();
            ImGui::CreateContext();
            ImGui::StyleColorsDark();

            ImGui_ImplGlfw_InitForOpenGL(s_window, true);
            ImGui_ImplOpenGL3_Init("#version 330 core");

            glEnable(GL_CULL_FACE);
            glEnable(GL_DEPTH_TEST);
            glDepthFunc(GL_LESS);

            if ((GetResourcesPath() / "icon.png").exists()) {
                int w, h;
                const auto icon_data = SOIL_load_image((GetResourcesPath() / "icon.png").as_cstr(), &w, &h, nullptr, SOIL_LOAD_RGBA);
                GLFWimage icon_image = {
                    w, h, icon_data
                };
                glfwSetWindowIcon(s_window, 1, &icon_image);
                SOIL_free_image_data(icon_data);
            }

#ifndef NDEBUG
            const auto debug_title = std::format("{} | FogOfWar Engine - {}", s_window_title, GetVersion().to_string());
            glfwSetWindowTitle(s_window, debug_title.c_str());
#endif

            if (const auto lang = cl_lang->as_string(); lang.has_value()) {
                SetLanguage(lang.value());
            }
            s_initialized = true;
            return Success();
        }
        void Run() {
            double last_time = glfwGetTime();
            double time = 0.0;

            SetGameStateRunning();

            if (s_game_class != nullptr) {
                s_game_class->on_init();
            }

            while (GetGameState() == GameState::Running) {
                glfwPollEvents();

                if (glfwGetWindowAttrib(s_window, GLFW_ICONIFIED) != 0) {
                    ImGui_ImplGlfw_Sleep(10);
                    continue;
                }

                time = glfwGetTime();

                if (s_game_class != nullptr) {
                    s_game_class->on_update(time - last_time);
                }


                ImGui_ImplOpenGL3_NewFrame();
                ImGui_ImplGlfw_NewFrame();
                ImGui::NewFrame();

                if (s_game_class != nullptr) {
                    s_game_class->on_update_imgui(time - last_time);
                }

                Console::UpdateConsole();

                ImGui::Render();
                int display_w, display_h;
                glfwGetFramebufferSize(s_window, &display_w, &display_h);
                glClearColor(s_background_color.r, s_background_color.g, s_background_color.b, s_background_color.a);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                if (s_game_class != nullptr) {
                    s_game_class->on_render(time - last_time);
                }

                ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

                glfwSwapBuffers(s_window);
                last_time = time;

                if (glfwWindowShouldClose(s_window)) {
                    SetGameStateClosing();
                }
            }

            auto cfg_path = s_base_path / "cfg";
            if (!cfg_path.exists()) {
                Path::CreateDirectory(cfg_path, true);
            }

            if (s_game_class != nullptr) {
                s_game_class->on_close();
            }
            s_game_class = nullptr;

            Console::Terminate();

            Texture::UnloadPlaceHolder();
            Shader::UnloadPlaceHolder();
            Assets::ClearCache();

            ImGui_ImplOpenGL3_Shutdown();
            ImGui_ImplGlfw_Shutdown();
            ImGui::DestroyContext();

            SetGameStateTerminated();

            if (s_window != nullptr) {
                glfwDestroyWindow(s_window);
                s_window = nullptr;
            }
            glfwTerminate();

            if (GetGameState() == GameState::Crashed) {
                exit(GetGameExitCode());
            }
        }

        void SetWindowTitle(const String& title) {
            s_window_title = title;
#ifndef NDEBUG
            const auto debug_title = std::format("{} | FogOfWar Engine - {}", s_window_title, GetVersion().to_string());
            glfwSetWindowTitle(s_window, debug_title.c_str());
#else
            glfwSetWindowTitle(s_window, title.as_cstr());
#endif
        }
        void SetWindowPosition(const glm::ivec2& value) {
            glfwSetWindowPos(s_window, value.x, value.y);
        }
        void SetWindowSize(const glm::ivec2& value) {
            glfwSetWindowSize(s_window, value.x, value.y);
        }
        String GetWindowTitle() {
            return s_window_title;
        }
        glm::ivec2 GetWindowPosition() {
            glm::ivec2 result;
            glfwGetWindowPos(s_window, &result.x, &result.y);
            return result;
        }
        glm::ivec2 GetWindowSize() {
            glm::ivec2 result;
            glfwGetWindowSize(s_window, &result.x, &result.y);
            return result;
        }

        const Version& GetVersion() {
            return s_version;
        }
    }

    static void UpdateResolution(const CVarPtr& self) {
        if (Engine::s_window == nullptr) {
            return;
        }

        if (const auto res = self->as_vec2(); res.has_value()) {
            glfwSetWindowSize(Engine::s_window, res->x, res->y);
        }
    }
    static void UpdateWindowMode(const CVarPtr& self) {
        if (Engine::s_window == nullptr) {
            return;
        }
    }
    static void UpdateMonitorIndex(const CVarPtr& self) {
        if (Engine::s_window == nullptr) {
            return;
        }
    }
    static void UpdateVSync(const CVarPtr& self) {
        if (Engine::s_window == nullptr) {
            return;
        }
    }
    static void UpdateLanguage(const CVarPtr& self) {
        if (!Engine::s_initialized) {
            return;
        }
        if (const auto value = self->as_string(); value.has_value()) {
            Debug::Assert(SetLanguage(value.value()));
        }
    }
    static Result<> QuitCommand(const Vector<String>& args) {
        DISCARD(args);
        if (Engine::s_window == nullptr) {
            exit(GetGameExitCode());
        } else {
            glfwSetWindowShouldClose(Engine::s_window, GLFW_TRUE);
        }
        return Success();
    }

    namespace Input {
        struct Action {
            Type type;
            int keycode;
        };

        static Vector<State> s_keyboard_state;
        static Vector<State> s_mouse_btn_state;
        static HashMap<String, Action> s_actions;

        static glm::dvec2    s_mouse_position;
        static glm::dvec2    s_mouse_position_delta;
        static glm::dvec2    s_mouse_scroll;
        static bool          s_initialized = false;

        void Initialize() {
            if (s_initialized) return;

            s_keyboard_state.reserve(GLFW_KEY_LAST + 1);
            s_mouse_btn_state.reserve(8);
            glfwGetCursorPos(Engine::s_window, &s_mouse_position.x, &s_mouse_position.y);

            s_mouse_position_delta = glm::dvec2 { 0.0f };
            s_mouse_scroll         = glm::dvec2 { 0.0f };
            s_initialized          = true;

            glfwSetScrollCallback(Engine::s_window, [](GLFWwindow* window, const double x, const double y) {
                DISCARD(window);
                s_mouse_scroll = glm::dvec2 { x, y };
            });
        }
        void Poll() {
            for (int i = 0; i < GLFW_KEY_LAST + 1; ++i) {
                const auto state = glfwGetKey(Engine::s_window, i);
                auto& kb_state = s_keyboard_state.at(i);
                if (state == GLFW_PRESS || state == GLFW_REPEAT) {
                    if (kb_state == State::Pressed) {
                        kb_state = State::Down;
                    } else if (kb_state > State::Down) {
                        kb_state = State::Pressed;
                    }
                } else {
                    if (kb_state < State::Released) {
                        kb_state = State::Released;
                    } else if (kb_state == State::Released) {
                        kb_state = State::Up;
                    }
                }
            }
            for (int i = 0; i < 8; ++i) {
                const auto state = glfwGetMouseButton(Engine::s_window, i);
                auto& mb_state = s_mouse_btn_state.at(i);
                if (state == GLFW_PRESS || state == GLFW_REPEAT) {
                    if (mb_state == State::Pressed) {
                        mb_state = State::Down;
                    } else if (mb_state > State::Down) {
                        mb_state = State::Pressed;
                    }
                } else {
                    if (mb_state < State::Released) {
                        mb_state = State::Released;
                    } else if (mb_state == State::Released) {
                        mb_state = State::Up;
                    }
                }
            }

            const auto prev_mouse_pos = s_mouse_position;
            glfwGetCursorPos(Engine::s_window, &s_mouse_position.x, &s_mouse_position.y);
            s_mouse_position_delta = s_mouse_position - prev_mouse_pos;
        }

        Result<> CreateAction(const String& name, const Type type, const int value) {
            if (s_actions.contains(name)) {
                return Failure(std::format("Input action \"{}\" is already defined", name));
            }
            s_actions.emplace(name, Action { type, value });
            return Success();
        }

        bool ActionIsPressed(const String& action) {
            if (!s_actions.contains(action)) {
                Debug::LogError(std::format("Input action \"{}\" doesn't exists!", action));
                return false;
            }
            switch (const auto [ type, keycode ] = s_actions.at(action); type) {
                case Type::KeyboardKey: {
                    return s_keyboard_state.at(keycode) == State::Pressed;
                }
                case Type::MouseButton: {
                    return s_mouse_btn_state.at(keycode) == State::Pressed;
                }
                case Type::MouseMotion: {
                    return (keycode == 0 ? s_mouse_position_delta.x : s_mouse_position_delta.y) != 0.0f;
                }
                case Type::MouseWheel: {
                    return (keycode == 0 ? s_mouse_scroll.x : s_mouse_scroll.y) != 0.0f;
                }
            }
            return false;
        }
        bool ActionIsDown(const String& action) {
            if (!s_actions.contains(action)) {
                Debug::LogError(std::format("Input action \"{}\" doesn't exists!", action));
                return false;
            }
            switch (const auto [ type, keycode ] = s_actions.at(action); type) {
                case Type::KeyboardKey: {
                    return s_keyboard_state.at(keycode) == State::Pressed || s_keyboard_state.at(keycode) == State::Down;
                }
                case Type::MouseButton: {
                    return s_mouse_btn_state.at(keycode) == State::Pressed || s_mouse_btn_state.at(keycode) == State::Down;
                }
                case Type::MouseMotion: {
                    return (keycode == 0 ? s_mouse_position_delta.x : s_mouse_position_delta.y) != 0.0f;
                }
                case Type::MouseWheel: {
                    return (keycode == 0 ? s_mouse_scroll.x : s_mouse_scroll.y) != 0.0f;
                }
            }
            return false;
        }
        bool ActionIsReleased(const String& action) {
            if (!s_actions.contains(action)) {
                Debug::LogError(std::format("Input action \"{}\" doesn't exists!", action));
                return false;
            }
            switch (const auto [ type, keycode ] = s_actions.at(action); type) {
                case Type::KeyboardKey: {
                    return s_keyboard_state.at(keycode) == State::Released;
                }
                case Type::MouseButton: {
                    return s_mouse_btn_state.at(keycode) == State::Released;
                }
                case Type::MouseMotion: {
                    return (keycode == 0 ? s_mouse_position_delta.x : s_mouse_position_delta.y) == 0.0f;
                }
                case Type::MouseWheel: {
                    return (keycode == 0 ? s_mouse_scroll.x : s_mouse_scroll.y) == 0.0f;
                }
            }
            return false;
        }
        bool ActionIsUp(const String& action) {
            if (!s_actions.contains(action)) {
                Debug::LogError(std::format("Input action \"{}\" doesn't exists!", action));
                return false;
            }
            switch (const auto [ type, keycode ] = s_actions.at(action); type) {
                case Type::KeyboardKey: {
                    return s_keyboard_state.at(keycode) == State::Released || s_keyboard_state.at(keycode) == State::Up;
                }
                case Type::MouseButton: {
                    return s_mouse_btn_state.at(keycode) == State::Released || s_mouse_btn_state.at(keycode) == State::Up;
                }
                case Type::MouseMotion: {
                    return (keycode == 0 ? s_mouse_position_delta.x : s_mouse_position_delta.y) == 0.0f;
                }
                case Type::MouseWheel: {
                    return (keycode == 0 ? s_mouse_scroll.x : s_mouse_scroll.y) == 0.0f;
                }
            }
            return false;
        }
        float GetAxis(const String& positive_action, const String& negative_action, const float multiplier) {
            return GetAxis(positive_action, multiplier) - GetAxis(negative_action, multiplier);
        }
        float GetAxis(const String& action, const float multiplier) {
            if (!s_actions.contains(action)) {
                Debug::LogError(std::format("Input action \"{}\" doesn't exists!", action));
                return 0.0f;
            }

            switch (const auto [ type, keycode ] = s_actions.at(action); type) {
                case Type::KeyboardKey: {
                     return s_keyboard_state.at(keycode) < State::Released ? multiplier : 0.0f;
                }
                case Type::MouseButton: {
                     return s_mouse_btn_state.at(keycode) < State::Released ? multiplier : 0.0f;
                }
                case Type::MouseMotion: {
                    return (keycode == 0 ? s_mouse_position_delta.x : s_mouse_position_delta.y) * multiplier;
                }
                case Type::MouseWheel: {
                    return (keycode == 0 ? s_mouse_scroll.x : s_mouse_scroll.y) * multiplier;
                }
            }
            return 0.0f;
        }

        bool KeyIsPressed(const KeyCode key) {
            return s_keyboard_state.at(static_cast<int>(key)) == State::Pressed;
        }
        bool KeyIsDown(const KeyCode key) {
            return s_keyboard_state.at(static_cast<int>(key)) == State::Pressed || s_keyboard_state.at(static_cast<int>(key)) == State::Down;
        }
        bool KeyIsReleased(const KeyCode key) {
            return s_keyboard_state.at(static_cast<int>(key)) == State::Released;
        }
        bool KeyIsUp(const KeyCode key) {
            return s_keyboard_state.at(static_cast<int>(key)) == State::Released || s_keyboard_state.at(static_cast<int>(key)) == State::Up;
        }

        bool MouseIsPressed(const MouseButton button) {
            return s_mouse_btn_state.at(static_cast<int>(button)) == State::Pressed;
        }
        bool MouseIsDown(const MouseButton button) {
            return s_mouse_btn_state.at(static_cast<int>(button)) == State::Pressed || s_mouse_btn_state.at(static_cast<int>(button)) == State::Down;
        }
        bool MouseIsReleased(const MouseButton button) {
            return s_mouse_btn_state.at(static_cast<int>(button)) == State::Released;
        }
        bool MouseIsUp(const MouseButton button) {
            return s_mouse_btn_state.at(static_cast<int>(button)) == State::Released || s_mouse_btn_state.at(static_cast<int>(button)) == State::Up;
        }

        glm::vec2 MousePosition() {
            return s_mouse_position;
        }
        glm::vec2 MouseMovement() {
            return s_mouse_position_delta;
        }
    }
}
