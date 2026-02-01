#include "fow/Engine.hpp"
#include "fow/Engine/Convar.hpp"

#include "fow/Renderer.hpp"
#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include "SOIL2.h"

#include <fow/Engine/ImGui.hpp>

namespace fow {
    static void UpdateResolution(const CVarPtr& self);
    static void UpdateWindowMode(const CVarPtr& self);
    static void UpdateMonitorIndex(const CVarPtr& self);
    static void UpdateVSync(const CVarPtr& self);
    static void UpdateMSAA(const CVarPtr& self);
    static void UpdateLanguage(const CVarPtr& self);
    static Result<> QuitCommand(const Vector<String>& args);
    static Result<> CreateActionCommand(const Vector<String>& args);
    static Result<> RemoveActionCommand(const Vector<String>& args);
    static Result<> ToggleConsoleCommand(const Vector<String>& args);

    const auto vid_resolution  = CVar::Create("vid_resolution",  glm::vec2(1280, 720),  CVarFlags::UserSettings | CVarFlags::SaveToConfig, &UpdateResolution);
    const auto vid_window_mode = CVar::Create("vid_window_mode", "Windowed",            CVarFlags::UserSettings | CVarFlags::SaveToConfig, &UpdateWindowMode);
    const auto vid_monitor_idx = CVar::Create("vid_monitor_idx", 0,                     CVarFlags::UserSettings | CVarFlags::SaveToConfig, &UpdateMonitorIndex);
    const auto vid_vsync       = CVar::Create("vid_vsync",       false,                 CVarFlags::UserSettings | CVarFlags::SaveToConfig, &UpdateVSync);
    const auto r_msaa          = CVar::Create("r_msaa",          0,                     CVarFlags::UserSettings | CVarFlags::SaveToConfig, &UpdateMSAA);
    const auto cl_lang         = CVar::Create("cl_lang",         "en_us",               CVarFlags::UserSettings | CVarFlags::SaveToConfig, &UpdateLanguage);
    const auto quit            = CVar::Create("quit",            &QuitCommand,          CVarFlags::Default);
    const auto create_action   = CVar::Create("create_action",   &CreateActionCommand,  CVarFlags::Default);
    const auto remove_action   = CVar::Create("remove_action",   &RemoveActionCommand,  CVarFlags::Default);
    const auto toggle_console  = CVar::Create("toggle_console",  &ToggleConsoleCommand, CVarFlags::Default);

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

            Path::CurrentDir(s_base_path);

            Assets::Initialize(s_game_class->base_data_path(), s_game_class->game_data_archives(), s_game_class->mod_data_path());
            Debug::Assert(Console::Initialize());

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
            Debug::Assert(LoadLanguageFiles());

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

            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
            const auto msaa = std::min(r_msaa->as_int().value_or(0), 16);
            if (msaa > 0) {
                glfwWindowHint(GLFW_SAMPLES, msaa);
            }
            s_window = glfwCreateWindow(resolution.x, resolution.y, title.as_cstr(), monitor, nullptr);

            if (s_window == nullptr) {
                const char* message;
                int code = glfwGetError(&message);
                glfwTerminate();
                return Failure(std::format("Failed to create window: \"{}\" (code {})", message, code));
            }

            Input::Initialize();
            s_window_title = title;
            glfwMakeContextCurrent(s_window);
            if (const auto result = Renderer::Initialize(s_base_path, reinterpret_cast<GLADloadproc>(glfwGetProcAddress)); !result.has_value()) {
                glfwDestroyWindow(s_window);
                glfwTerminate();
                return Failure(result.error());
            }
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

            if (msaa > 0) {
                glEnable(GL_MULTISAMPLE);
            } else {
                glDisable(GL_MULTISAMPLE);
            }

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
                Debug::Assert(SetLanguage(lang.value()));
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
                Input::Poll();

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
                Renderer::Clear(s_background_color);

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
            Shader::UnloadShaderCache();
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

    void UpdateMSAA(const CVarPtr& self) {
        // TODO: Show message to the user to restart the game
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
            for (int i = 0; i < s_keyboard_state.capacity(); ++i) {
                s_keyboard_state.emplace_back(State::Up);
            }
            s_mouse_btn_state.reserve(8);
            for (int i = 0; i < s_mouse_btn_state.capacity(); ++i) {
                s_mouse_btn_state.emplace_back(State::Up);
            }
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

        Result<> CreateAction(const String& name, const Action action) {
            if (s_actions.contains(name)) {
                return Failure(std::format("Input action \"{}\" is already defined", name));
            }
            s_actions.emplace(name, action);
            return Success();
        }
        Result<> CreateAction(const String& name, const Type type, const int value) {
            if (s_actions.contains(name)) {
                return Failure(std::format("Input action \"{}\" is already defined", name));
            }
            s_actions.emplace(name, Action { type, value });
            return Success();
        }

        void DeleteAction(const String& name) {
            if (s_actions.contains(name)) {
                s_actions.erase(name);
            }
        }

        const HashMap<String, Action>& AvailableActions() {
            return s_actions;
        }
        Result<Action> StringToAction(const String& key) {
            if (key.starts_with("mb_", StringCompareType::CaseInsensitive)) {
                const auto result = StringToInt<int>(key.substr(2));
                if (!result.has_value()) {
                    return Failure(std::format("Invalid key code \"{}\"", key));
                }
                return Success<Action>(Action { Type::MouseButton, result.value() });
            }
            if (key.equals("mouse_x", StringCompareType::CaseInsensitive)) {
                return Success<Action>(Action { Type::MouseMotion, 0 });
            }
            if (key.equals("mouse_y", StringCompareType::CaseInsensitive)) {
                return Success<Action>(Action { Type::MouseMotion, 1 });
            }
            if (key.equals("wheel_x", StringCompareType::CaseInsensitive)) {
                return Success<Action>(Action { Type::MouseWheel, 0 });
            }
            if (key.equals("wheel_y", StringCompareType::CaseInsensitive)) {
                return Success<Action>(Action { Type::MouseWheel, 1 });
            }
            if (key.starts_with("kp_", StringCompareType::CaseInsensitive)) {
                const auto result = StringToInt<unsigned>(String(key).remove_prefix("kp_"));
                if (result.has_value() && result.value() < 10) {
                    return Success<Action>(Action { Type::KeyboardKey, static_cast<int>(GLFW_KEY_KP_0 + result.value()) });
                }
                if (key.equals("kp_decimal", StringCompareType::CaseInsensitive)) {
                    return Success<Action>(Action { Type::KeyboardKey, GLFW_KEY_KP_DECIMAL });
                }
                if (key.equals("kp_divide", StringCompareType::CaseInsensitive)) {
                    return Success<Action>(Action { Type::KeyboardKey, GLFW_KEY_KP_DIVIDE });
                }
                if (key.equals("kp_multiply", StringCompareType::CaseInsensitive)) {
                    return Success<Action>(Action { Type::KeyboardKey, GLFW_KEY_KP_MULTIPLY });
                }
                if (key.equals("kp_subtract", StringCompareType::CaseInsensitive)) {
                    return Success<Action>(Action { Type::KeyboardKey, GLFW_KEY_KP_SUBTRACT });
                }
                if (key.equals("kp_add", StringCompareType::CaseInsensitive)) {
                    return Success<Action>(Action { Type::KeyboardKey, GLFW_KEY_KP_ADD });
                }
                if (key.equals("kp_enter", StringCompareType::CaseInsensitive)) {
                    return Success<Action>(Action { Type::KeyboardKey, GLFW_KEY_KP_ENTER });
                }
                if (key.equals_any({ "kp_equal", "kp_equals" }, StringCompareType::CaseInsensitive)) {
                    return Success<Action>(Action { Type::KeyboardKey, GLFW_KEY_KP_EQUAL });
                }
            }
            if (key.size() == 1) {
                if (std::isdigit(key[0])) {
                    return Success<Action>(Action { Type::KeyboardKey, GLFW_KEY_0 + (key[0] - '0') });
                }
                if (std::isalpha(key[0])) {
                    return Success<Action>(Action { Type::KeyboardKey, GLFW_KEY_A + (std::toupper(key[0]) - 'A') });
                }
                return Failure(std::format("Invalid key code \"{}\"", key));
            }
            if (key[0] == 'F' || key[0] == 'f') {
                const auto result = StringToInt<int>(key.substr(1));
                if (result.has_value() && result.value() > 0 && result.value() <= 25) {
                    return Success<Action>(Action { Type::KeyboardKey, GLFW_KEY_F1 + result.value() });
                }
            }

            if (key.equals("space", StringCompareType::CaseInsensitive)) {
                return Success<Action>(Action { Type::KeyboardKey, GLFW_KEY_SPACE });
            }
            if (key.equals_any({ "enter", "return" }, StringCompareType::CaseInsensitive)) {
                return Success<Action>(Action { Type::KeyboardKey, GLFW_KEY_ENTER });
            }
            if (key.equals("apostrophe", StringCompareType::CaseInsensitive)) {
                return Success<Action>(Action { Type::KeyboardKey, GLFW_KEY_APOSTROPHE });
            }
            if (key.equals("comma", StringCompareType::CaseInsensitive)) {
                return Success<Action>(Action { Type::KeyboardKey, GLFW_KEY_COMMA });
            }
            if (key.equals_any({ "minus", "negative" }, StringCompareType::CaseInsensitive)) {
                return Success<Action>(Action { Type::KeyboardKey, GLFW_KEY_MINUS });
            }
            if (key.equals("period", StringCompareType::CaseInsensitive)) {
                return Success<Action>(Action { Type::KeyboardKey, GLFW_KEY_PERIOD });
            }
            if (key.equals("slash", StringCompareType::CaseInsensitive)) {
                return Success<Action>(Action { Type::KeyboardKey, GLFW_KEY_SLASH });
            }
            if (key.equals("semicolon", StringCompareType::CaseInsensitive)) {
                return Success<Action>(Action { Type::KeyboardKey, GLFW_KEY_SEMICOLON });
            }
            if (key.equals_any({ "equal", "equals" }, StringCompareType::CaseInsensitive)) {
                return Success<Action>(Action { Type::KeyboardKey, GLFW_KEY_EQUAL });
            }
            if (key.equals("left_bracket", StringCompareType::CaseInsensitive)) {
                return Success<Action>(Action { Type::KeyboardKey, GLFW_KEY_LEFT_BRACKET });
            }
            if (key.equals("right_bracket", StringCompareType::CaseInsensitive)) {
                return Success<Action>(Action { Type::KeyboardKey, GLFW_KEY_RIGHT_BRACKET });
            }
            if (key.equals("grave_accent", StringCompareType::CaseInsensitive)) {
                return Success<Action>(Action { Type::KeyboardKey, GLFW_KEY_GRAVE_ACCENT });
            }
            if (key.equals("backslash", StringCompareType::CaseInsensitive)) {
                return Success<Action>(Action { Type::KeyboardKey, GLFW_KEY_BACKSLASH });
            }
            if (key.equals("escape", StringCompareType::CaseInsensitive)) {
                return Success<Action>(Action { Type::KeyboardKey, GLFW_KEY_ESCAPE });
            }
            if (key.equals("tab", StringCompareType::CaseInsensitive)) {
                return Success<Action>(Action { Type::KeyboardKey, GLFW_KEY_TAB });
            }
            if (key.equals("backspace", StringCompareType::CaseInsensitive)) {
                return Success<Action>(Action { Type::KeyboardKey, GLFW_KEY_BACKSPACE });
            }
            if (key.equals("insert", StringCompareType::CaseInsensitive)) {
                return Success<Action>(Action { Type::KeyboardKey, GLFW_KEY_INSERT });
            }
            if (key.equals("delete", StringCompareType::CaseInsensitive)) {
                return Success<Action>(Action { Type::KeyboardKey, GLFW_KEY_DELETE });
            }
            if (key.equals("right", StringCompareType::CaseInsensitive)) {
                return Success<Action>(Action { Type::KeyboardKey, GLFW_KEY_RIGHT });
            }
            if (key.equals("left", StringCompareType::CaseInsensitive)) {
                return Success<Action>(Action { Type::KeyboardKey, GLFW_KEY_LEFT });
            }
            if (key.equals("down", StringCompareType::CaseInsensitive)) {
                return Success<Action>(Action { Type::KeyboardKey, GLFW_KEY_DOWN });
            }
            if (key.equals("up", StringCompareType::CaseInsensitive)) {
                return Success<Action>(Action { Type::KeyboardKey, GLFW_KEY_UP });
            }
            if (key.equals("page_up", StringCompareType::CaseInsensitive)) {
                return Success<Action>(Action { Type::KeyboardKey, GLFW_KEY_PAGE_UP });
            }
            if (key.equals("page_down", StringCompareType::CaseInsensitive)) {
                return Success<Action>(Action { Type::KeyboardKey, GLFW_KEY_PAGE_DOWN });
            }
            if (key.equals("home", StringCompareType::CaseInsensitive)) {
                return Success<Action>(Action { Type::KeyboardKey, GLFW_KEY_HOME });
            }
            if (key.equals("end", StringCompareType::CaseInsensitive)) {
                return Success<Action>(Action { Type::KeyboardKey, GLFW_KEY_END });
            }
            if (key.equals_any({ "caps_lock", "capslock" }, StringCompareType::CaseInsensitive)) {
                return Success<Action>(Action { Type::KeyboardKey, GLFW_KEY_CAPS_LOCK });
            }
            if (key.equals_any({ "scroll_lock", "scrolllock" }, StringCompareType::CaseInsensitive)) {
                return Success<Action>(Action { Type::KeyboardKey, GLFW_KEY_SCROLL_LOCK });
            }
            if (key.equals_any({ "num_lock", "numlock" }, StringCompareType::CaseInsensitive)) {
                return Success<Action>(Action { Type::KeyboardKey, GLFW_KEY_NUM_LOCK });
            }
            if (key.equals("printscreen", StringCompareType::CaseInsensitive)) {
                return Success<Action>(Action { Type::KeyboardKey, GLFW_KEY_PRINT_SCREEN });
            }
            if (key.equals("pause", StringCompareType::CaseInsensitive)) {
                return Success<Action>(Action { Type::KeyboardKey, GLFW_KEY_PAUSE });
            }
            if (key.equals("left_shift", StringCompareType::CaseInsensitive)) {
                return Success<Action>(Action { Type::KeyboardKey, GLFW_KEY_LEFT_SHIFT });
            }
            if (key.equals("left_ctrl", StringCompareType::CaseInsensitive)) {
                return Success<Action>(Action { Type::KeyboardKey, GLFW_KEY_LEFT_CONTROL });
            }
            if (key.equals("left_alt", StringCompareType::CaseInsensitive)) {
                return Success<Action>(Action { Type::KeyboardKey, GLFW_KEY_LEFT_ALT });
            }
            if (key.equals("right_shift", StringCompareType::CaseInsensitive)) {
                return Success<Action>(Action { Type::KeyboardKey, GLFW_KEY_RIGHT_SHIFT });
            }
            if (key.equals("right_ctrl", StringCompareType::CaseInsensitive)) {
                return Success<Action>(Action { Type::KeyboardKey, GLFW_KEY_RIGHT_CONTROL });
            }
            if (key.equals("right_alt", StringCompareType::CaseInsensitive)) {
                return Success<Action>(Action { Type::KeyboardKey, GLFW_KEY_RIGHT_ALT });
            }
            if (key.equals("menu", StringCompareType::CaseInsensitive)) {
                return Success<Action>(Action { Type::KeyboardKey, GLFW_KEY_MENU });
            }
            return Failure(std::format("Invalid key code \"{}\"", key));
        }
        Result<String> ActionToString(Action action) {
            switch (action.type) {
                case Type::KeyboardKey: {
                    if (action.keycode >= GLFW_KEY_0 && action.keycode <= GLFW_KEY_9) {
                        return Success<String>(std::to_string(action.keycode - GLFW_KEY_0));
                    }
                    if (action.keycode >= GLFW_KEY_A && action.keycode <= GLFW_KEY_Z) {
                        return Success<String>(String { static_cast<char>(action.keycode - GLFW_KEY_A + 'A'), 1 });
                    }
                    if (action.keycode >= GLFW_KEY_KP_0 && action.keycode <= GLFW_KEY_KP_9) {
                        return Success<String>(std::format("kp_{}", action.keycode - GLFW_KEY_KP_0));
                    }
                    if (action.keycode >= GLFW_KEY_F1 && action.keycode <= GLFW_KEY_F25) {
                        return Success<String>(std::format("F{}", action.keycode - GLFW_KEY_F1 + 1));
                    }

                    switch (action.keycode) {
                        case GLFW_KEY_SPACE: return Success<String>("space");
                        case GLFW_KEY_APOSTROPHE: return Success<String>("apostrophe");
                        case GLFW_KEY_COMMA: return Success<String>("comma");
                        case GLFW_KEY_MINUS: return Success<String>("minus");
                        case GLFW_KEY_PERIOD: return Success<String>("period");
                        case GLFW_KEY_SLASH: return Success<String>("slash");
                        case GLFW_KEY_SEMICOLON: return Success<String>("semicolon");
                        case GLFW_KEY_EQUAL: return Success<String>("equal");
                        case GLFW_KEY_LEFT_BRACKET: return Success<String>("left_bracket");
                        case GLFW_KEY_BACKSLASH: return Success<String>("backslash");
                        case GLFW_KEY_RIGHT_BRACKET: return Success<String>("right_bracket");
                        case GLFW_KEY_GRAVE_ACCENT: return Success<String>("grave_accent");
                        case GLFW_KEY_ESCAPE: return Success<String>("escape");
                        case GLFW_KEY_ENTER: return Success<String>("enter");
                        case GLFW_KEY_TAB: return Success<String>("tab");
                        case GLFW_KEY_BACKSPACE: return Success<String>("backspace");
                        case GLFW_KEY_INSERT: return Success<String>("insert");
                        case GLFW_KEY_DELETE: return Success<String>("delete");
                        case GLFW_KEY_RIGHT: return Success<String>("right");
                        case GLFW_KEY_LEFT: return Success<String>("left");
                        case GLFW_KEY_DOWN: return Success<String>("down");
                        case GLFW_KEY_UP: return Success<String>("up");
                        case GLFW_KEY_PAGE_UP: return Success<String>("page_up");
                        case GLFW_KEY_PAGE_DOWN: return Success<String>("page_down");
                        case GLFW_KEY_HOME: return Success<String>("home");
                        case GLFW_KEY_END: return Success<String>("end");
                        case GLFW_KEY_CAPS_LOCK: return Success<String>("caps_lock");
                        case GLFW_KEY_SCROLL_LOCK: return Success<String>("scroll_lock");
                        case GLFW_KEY_NUM_LOCK: return Success<String>("num_lock");
                        case GLFW_KEY_PRINT_SCREEN: return Success<String>("printscreen");
                        case GLFW_KEY_PAUSE: return Success<String>("pause");
                        case GLFW_KEY_KP_DECIMAL: return Success<String>("kp_decimal");
                        case GLFW_KEY_KP_DIVIDE: return Success<String>("kp_divide");
                        case GLFW_KEY_KP_MULTIPLY: return Success<String>("kp_multiply");
                        case GLFW_KEY_KP_SUBTRACT: return Success<String>("kp_subtract");
                        case GLFW_KEY_KP_ADD: return Success<String>("kp_add");
                        case GLFW_KEY_KP_ENTER: return Success<String>("kp_enter");
                        case GLFW_KEY_KP_EQUAL: return Success<String>("kp_equal");
                        case GLFW_KEY_LEFT_SHIFT: return Success<String>("left_shift");
                        case GLFW_KEY_LEFT_CONTROL: return Success<String>("left_ctrl");
                        case GLFW_KEY_LEFT_ALT: return Success<String>("left_alt");
                        case GLFW_KEY_RIGHT_SHIFT: return Success<String>("right_shift");
                        case GLFW_KEY_RIGHT_CONTROL: return Success<String>("right_ctrl");
                        case GLFW_KEY_RIGHT_ALT: return Success<String>("right_alt");
                        case GLFW_KEY_MENU: return Success<String>("menu");
                        default: return Failure(std::format("Invalid key code {}", action.keycode));
                    }
                }
                case Type::MouseButton: {
                    return Success<String>(std::format("mb_{}", action.keycode));
                }
                case Type::MouseMotion: {
                    return Success<String>(action.keycode == 0 ? "mouse_x" : "mouse_y");
                }
                case Type::MouseWheel: {
                    return Success<String>(action.keycode == 0 ? "wheel_x" : "wheel_y");
                }
            }
            return Failure(std::format("Invalid key type \"{}\"", static_cast<int>(action.type)));
        }

        void SetCursorMode(const CursorMode mode) {
            glfwSetInputMode(Engine::s_window, GLFW_CURSOR, mode);
        }
        CursorMode GetCursorMode() {
            return static_cast<CursorMode>(glfwGetInputMode(Engine::s_window, GLFW_CURSOR));
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

    static Result<> CreateActionCommand(const Vector<String>& args) {
        if (args.size() == 1) {
            const auto& name = args.at(0);
           if (Input::s_actions.contains(args.at(0))) {
               const auto& action = Input::s_actions.at(name);
               Debug::LogInfo(std::format("{} = {}", name, ActionToString(action).value_or("???")));
               return Success();
           }
        } else if (args.size() < 2) {
            return Failure("Usage: bind <name> <key>");
        }

        const auto action = Input::StringToAction(args.at(1));
        if (!action.has_value()) {
            return Failure(action.error());
        }
        Input::CreateAction(args.at(0), action.value());

        return Success();
    }
    static Result<> RemoveActionCommand(const Vector<String>& args) {
        if (args.size() < 1) {
            return Failure("Usage: unbind <name>");
        }
        Input::DeleteAction(args.at(0));
        return Success();
    }

    static Result<> ToggleConsoleCommand(const Vector<String>& args) {
        DISCARD(args);
        Console::ToggleConsoleVisible();
        return Success();
    }
}
