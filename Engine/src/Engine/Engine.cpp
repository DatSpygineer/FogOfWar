#define FOW_INCLUDE_IMGUI
#include "fow/Engine.hpp"
#include "fow/Engine/Convar.hpp"

#include "fow/Renderer/GL.hpp"
#include "fow/Renderer.hpp"

#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>

#include "SOIL2.h"

#include "rfl/json/write.hpp"

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
    static Result<> SetSceneCommand(const Vector<String>& args);

#ifndef NDEBUG
    static void GLDebugMessageCallback(GLenum source,GLenum type,GLuint id,GLenum severity,GLsizei length,const GLchar *message,const void *userParam);
#endif

    const auto vid_resolution      = CVar::Create("vid_resolution",      Vector2(1280, 720),    CVarFlags::UserSettings | CVarFlags::SaveToConfig, &UpdateResolution);
    const auto vid_window_mode     = CVar::Create("vid_window_mode",     "Windowed",            CVarFlags::UserSettings | CVarFlags::SaveToConfig, &UpdateWindowMode);
    const auto vid_monitor_idx     = CVar::Create("vid_monitor_idx",     0,                     CVarFlags::UserSettings | CVarFlags::SaveToConfig, &UpdateMonitorIndex);
    const auto vid_vsync           = CVar::Create("vid_vsync",           false,                 CVarFlags::UserSettings | CVarFlags::SaveToConfig, &UpdateVSync);
    const auto r_msaa              = CVar::Create("r_msaa",              0,                     CVarFlags::UserSettings | CVarFlags::SaveToConfig, &UpdateMSAA);
    const auto cl_lang             = CVar::Create("cl_lang",             "en_us",               CVarFlags::UserSettings | CVarFlags::SaveToConfig, &UpdateLanguage);
    const auto quit                = CVar::Create("quit",                &QuitCommand,          CVarFlags::Default);
    const auto input_create_action = CVar::Create("input_create_action", &CreateActionCommand,  CVarFlags::Default);
    const auto input_remove_action = CVar::Create("input_remove_action", &RemoveActionCommand,  CVarFlags::Default);
    const auto toggle_console      = CVar::Create("toggle_console",      &ToggleConsoleCommand, CVarFlags::Default);
    const auto set_scene           = CVar::Create("set_scene",           &SetSceneCommand,      CVarFlags::Default);

    namespace Input {
        static void UpdateMouseWheel(float x, float y);
    }

    namespace Engine {
        static bool s_initialized = false;
        static SDL_Window* s_window = nullptr;
        static SDL_GLContext s_glContext = nullptr;
        static String s_window_title = "FogOfWar";
        static Color s_background_color = { 0.25f, 0.5f, 1.0f };
        static SharedPtr<Game> s_game_class = nullptr;
        static Path s_base_path = Path::CurrentDir();
        static const auto s_version = Version { 0, 1, 0 };
        static ScenePtr s_scene = nullptr;

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

        Result<> Initialize(int argc, os_char_t** argv, const Function<std::shared_ptr<Game>()>& game_class_ctor) {
            if (s_initialized) {
                return Failure("Engine is already initialized!");
            }

            s_base_path = Path(argv[0]).parent();
            Debug::Initialize(s_base_path / "logs");
            s_game_class = std::move(game_class_ctor());

            s_window_title = s_game_class->title();

            Path::CurrentDir(s_base_path);

            Debug::AssertFatal(Assets::Initialize(s_base_path / "data", s_game_class->game_data_archives(), s_game_class->allow_mods() ? Some(s_base_path / "mods") : None()));

            Debug::Assert(Console::Initialize());

            HashMap<String, Vector<String>> args = ParseArgs(argc, argv);
            Debug::Assert(LoadLanguageFiles());

            if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS)) {
                return Failure(std::format("Failed to initialize SDL: {}", SDL_GetError()));
            }

            Vector2 resolution   = vid_resolution->as_vec2().value_or(Vector2 { 1280, 720 });
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
                        resolution = Vector2i(w.value(), h.value());
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

            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
            const auto msaa = std::min(r_msaa->as_int().value_or(0), 16);
            if (msaa > 0) {
                SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, msaa);
            }

            SDL_WindowFlags flags = SDL_WINDOW_OPENGL;

            if (window_mode == WindowMode::Fullscreen) {
                flags |= SDL_WINDOW_FULLSCREEN;
            }

            s_window = SDL_CreateWindow(s_window_title.as_cstr(), resolution.x, resolution.y,
                flags
            );

            if (s_window == nullptr) {
                String error = SDL_GetError();
                SDL_Quit();
                return Failure(std::format("Failed to create window: \"{}\"", error));
            }

            if (monitor_index < 0) {
                monitor_index = 0;
            }

            SDL_Rect display_rect;
            if (SDL_GetDisplayBounds(monitor_index, &display_rect)) {
                SDL_SetWindowPosition(s_window, display_rect.x, display_rect.y);
            }

            s_glContext = SDL_GL_CreateContext(s_window);
            if (s_glContext == nullptr) {
                String error = SDL_GetError();
                SDL_DestroyWindow(s_window);
                SDL_Quit();
                return Failure(std::format("Failed to create OpenGL context: \"{}\"", error));
            }

            Input::Initialize();
            if (const auto result = Renderer::Initialize(s_base_path, msaa, reinterpret_cast<void*(*)(const char*)>(SDL_GL_GetProcAddress)); !result.has_value()) {
                SDL_GL_DestroyContext(s_glContext);
                SDL_DestroyWindow(s_window);
                SDL_Quit();
                return Failure(result.error());
            }
            Renderer::EnableBlend(true);
            Renderer::SetViewport(0.0f, 0.0f, resolution.x, resolution.y);

            SDL_GL_SetSwapInterval(vsync_enabled ? 1 : 0);

            IMGUI_CHECKVERSION();
            ImGui::CreateContext();
            ImGui::StyleColorsDark();

            ImGui_ImplSDL3_InitForOpenGL(s_window, s_glContext);
            ImGui_ImplOpenGL3_Init("#version 330 core");

            if ((GetResourcesPath() / "icon.png").exists()) {
                int w, h;
                const auto icon_data = SOIL_load_image((GetResourcesPath() / "icon.png").as_cstr(), &w, &h, nullptr, SOIL_LOAD_RGBA);
                SDL_Surface* icon_surf = SDL_CreateSurface(w, h, SDL_PIXELFORMAT_RGBA8888);
                icon_surf->pixels = icon_data;
                SDL_SetWindowIcon(s_window, icon_surf);
                SOIL_free_image_data(icon_data);
            }

#ifndef NDEBUG
            const auto debug_title = std::format("{} | FogOfWar Engine - {}", s_window_title, GetVersion().to_string());
            SDL_SetWindowTitle(s_window, debug_title.c_str());

            glDebugMessageCallback(&GLDebugMessageCallback, nullptr);
#endif

            if (const auto lang = cl_lang->as_string(); lang.has_value()) {
                Debug::Assert(SetLanguage(lang.value()));
            }
            s_initialized = true;
            return Success();
        }
        void Run() {
            double last_time = Time();
            double time = 0.0;

            SetGameStateRunning();

            if (s_game_class != nullptr) {
                s_game_class->on_init();
            }

            if (s_scene != nullptr) {
                s_scene->spawn();
            }

            SDL_Event e;
            while (GetGameState() == GameState::Running) {
                while (SDL_PollEvent(&e)) {
                    switch (e.type) {
                        case SDL_EVENT_QUIT: SetGameStateClosing(); break;
                        case SDL_EVENT_WINDOW_RESIZED:
                        case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED: {
                            const auto width = e.window.data1;
                            const auto height = e.window.data2;
                            Renderer::SetViewport(0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height));
                            if (s_game_class != nullptr) {
                                s_game_class->on_window_resized(Vector2i { width, height });
                            }
                        } break;
                        case SDL_EVENT_MOUSE_WHEEL: {
                            Input::UpdateMouseWheel(e.wheel.x, e.wheel.y);
                        } break;
                    }
                }
                Input::Poll();

                time = Time();

                if (s_game_class != nullptr) {
                    s_game_class->on_update(time - last_time);
                }
                if (s_scene != nullptr) {
                    s_scene->update(time - last_time);
                }

                ImGui_ImplOpenGL3_NewFrame();
                ImGui_ImplSDL3_NewFrame();
                ImGui::NewFrame();

                if (s_game_class != nullptr) {
                    s_game_class->on_update_imgui(time - last_time);
                }

                Console::UpdateConsole();

                ImGui::Render();
                int display_w, display_h;
                SDL_GetWindowSize(s_window, &display_w, &display_h);
                Renderer::Clear(s_background_color);

                if (s_game_class != nullptr) {
                    s_game_class->on_render(time - last_time);
                }

                RenderQueue::Render();

                ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

                SDL_GL_SwapWindow(s_window);
                last_time = time;
            }

            if (s_scene != nullptr) {
                s_scene->destroy_all();
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
            ImGui_ImplSDL3_Shutdown();
            ImGui::DestroyContext();

            SetGameStateTerminated();

            if (s_glContext != nullptr) {
                SDL_GL_DestroyContext(s_glContext);
                s_glContext = nullptr;
            }
            if (s_window != nullptr) {
                SDL_DestroyWindow(s_window);
                s_window = nullptr;
            }
            SDL_Quit();

            if (GetGameState() == GameState::Crashed) {
                exit(GetGameExitCode());
            }
        }

        void SetWindowTitle(const String& title) {
            s_window_title = title;
#ifndef NDEBUG
            const auto debug_title = std::format("{} | FogOfWar Engine - {}", s_window_title, GetVersion().to_string());
            SDL_SetWindowTitle(s_window, debug_title.c_str());
#else
            SDL_SetWindowTitle(s_window, title.as_cstr());
#endif
        }
        void SetWindowPosition(const Vector2i& value) {
            SDL_SetWindowPosition(s_window, value.x, value.y);
        }
        void SetWindowSize(const Vector2i& value) {
            SDL_SetWindowSize(s_window, value.x, value.y);
        }
        String GetWindowTitle() {
            return s_window_title;
        }
        Vector2i GetWindowPosition() {
            Vector2i result;
            SDL_GetWindowPosition(s_window, &result.x, &result.y);
            return result;
        }
        Vector2i GetWindowSize() {
            Vector2i result;
            SDL_GetWindowSize(s_window, &result.x, &result.y);
            return result;
        }

        const Version& GetVersion() {
            return s_version;
        }

        void SetScene(const ScenePtr& scene) {
            if (s_scene != nullptr) {
                s_scene->destroy_all();
            }

            s_scene = scene;

            if (s_scene != nullptr && GetGameState() == GameState::Running) {
                s_scene->spawn();
            }
        }

        double Time() {
            return static_cast<double>(SDL_GetTicks()) / 1000.0;
        }
    }

    static void UpdateResolution(const CVarPtr& self) {
        if (Engine::s_window == nullptr) {
            return;
        }

        if (const auto res = self->as_vec2(); res.has_value()) {
            SDL_SetWindowSize(Engine::s_window, res->x, res->y);
        }
    }
    static void UpdateWindowMode(const CVarPtr& self) {
        if (Engine::s_window == nullptr) {
            return;
        }

        if (const auto mode = rfl::string_to_enum<WindowMode>(self->as_string()->as_std_str()); mode.has_value()) {
            switch (mode.value()) {
                case WindowMode::Windowed: {

                } break;
                case WindowMode::Fullscreen: {

                } break;
            }
        }
    }
    static void UpdateMonitorIndex(const CVarPtr& self) {
        if (Engine::s_window == nullptr) {
            return;
        }
        // TODO
    }
    static void UpdateVSync(const CVarPtr& self) {
        if (Engine::s_window == nullptr) {
            return;
        }
        // TODO
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
        FOW_DISCARD(args);
        if (Engine::s_window == nullptr) {
            exit(GetGameExitCode());
        } else {
            SDL_Event e;
            e.type = SDL_EVENT_QUIT;
            SDL_PushEvent(&e);
        }
        return Success();
    }

    namespace Input {
        static Vector<State> s_keyboard_state;
        static Vector<State> s_mouse_btn_state;
        static HashMap<String, Action> s_actions;

        static Vector2 s_mouse_position;
        static Vector2 s_mouse_position_delta;
        static Vector2 s_mouse_scroll;
        static bool    s_initialized = false;

        static void UpdateMouseWheel(const float x, const float y) {
            s_mouse_scroll = Vector2(x, y);
        }

        void Initialize() {
            if (s_initialized) return;

            s_keyboard_state.reserve(SDL_SCANCODE_COUNT + 1);
            for (int i = 0; i < s_keyboard_state.capacity(); ++i) {
                s_keyboard_state.emplace_back(State::Up);
            }
            s_mouse_btn_state.reserve(8);
            for (int i = 0; i < s_mouse_btn_state.capacity(); ++i) {
                s_mouse_btn_state.emplace_back(State::Up);
            }
            SDL_GetMouseState(&s_mouse_position.x, &s_mouse_position.y);

            s_mouse_position_delta = Vector2(0.0);
            s_mouse_scroll         = Vector2(0.0);
            s_initialized          = true;
        }
        void Poll() {
            const auto prev_pos = s_mouse_position;
            const auto mouse_state = SDL_GetMouseState(&s_mouse_position.x, &s_mouse_position.y);
            s_mouse_position_delta = s_mouse_position - prev_pos;

            int key_count;
            const auto key_states = SDL_GetKeyboardState(&key_count);
            for (int i = 0; i < key_count + 1; ++i) {
                const auto state = key_states[i];
                auto& kb_state = s_keyboard_state.at(i);
                if (state) {
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
                const auto state = (mouse_state >> i) & 1;
                auto& mb_state = s_mouse_btn_state.at(i);
                if (state) {
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
                    return Success<Action>(Action { Type::KeyboardKey, static_cast<int>(SDL_SCANCODE_KP_0 + result.value()) });
                }
                if (key.equals("kp_decimal", StringCompareType::CaseInsensitive)) {
                    return Success<Action>(Action { Type::KeyboardKey, SDL_SCANCODE_KP_DECIMAL });
                }
                if (key.equals("kp_divide", StringCompareType::CaseInsensitive)) {
                    return Success<Action>(Action { Type::KeyboardKey, SDL_SCANCODE_KP_DIVIDE });
                }
                if (key.equals("kp_multiply", StringCompareType::CaseInsensitive)) {
                    return Success<Action>(Action { Type::KeyboardKey, SDL_SCANCODE_KP_MULTIPLY });
                }
                if (key.equals("kp_subtract", StringCompareType::CaseInsensitive)) {
                    return Success<Action>(Action { Type::KeyboardKey, SDL_SCANCODE_KP_MINUS });
                }
                if (key.equals("kp_add", StringCompareType::CaseInsensitive)) {
                    return Success<Action>(Action { Type::KeyboardKey, SDL_SCANCODE_KP_PLUS });
                }
                if (key.equals("kp_enter", StringCompareType::CaseInsensitive)) {
                    return Success<Action>(Action { Type::KeyboardKey, SDL_SCANCODE_KP_ENTER });
                }
                if (key.equals_any({ "kp_equal", "kp_equals" }, StringCompareType::CaseInsensitive)) {
                    return Success<Action>(Action { Type::KeyboardKey, SDL_SCANCODE_KP_EQUALS });
                }
            }
            if (key.size() == 1) {
                if (std::isdigit(key[0])) {
                    return Success<Action>(Action { Type::KeyboardKey, SDL_SCANCODE_0 + (key[0] - '0') });
                }
                if (std::isalpha(key[0])) {
                    return Success<Action>(Action { Type::KeyboardKey, SDL_SCANCODE_A + (std::toupper(key[0]) - 'A') });
                }
                return Failure(std::format("Invalid key code \"{}\"", key));
            }
            if (key[0] == 'F' || key[0] == 'f') {
                const auto result = StringToInt<int>(key.substr(1));
                if (result.has_value() && result.value() > 0 && result.value() <= 25) {
                    return Success<Action>(Action { Type::KeyboardKey, SDL_SCANCODE_F1 + result.value() });
                }
            }

            if (key.equals("space", StringCompareType::CaseInsensitive)) {
                return Success<Action>(Action { Type::KeyboardKey, SDL_SCANCODE_SPACE });
            }
            if (key.equals_any({ "enter", "return" }, StringCompareType::CaseInsensitive)) {
                return Success<Action>(Action { Type::KeyboardKey, SDL_SCANCODE_RETURN });
            }
            if (key.equals("apostrophe", StringCompareType::CaseInsensitive)) {
                return Success<Action>(Action { Type::KeyboardKey, SDL_SCANCODE_APOSTROPHE });
            }
            if (key.equals("comma", StringCompareType::CaseInsensitive)) {
                return Success<Action>(Action { Type::KeyboardKey, SDL_SCANCODE_COMMA });
            }
            if (key.equals_any({ "minus", "negative" }, StringCompareType::CaseInsensitive)) {
                return Success<Action>(Action { Type::KeyboardKey, SDL_SCANCODE_MINUS });
            }
            if (key.equals("period", StringCompareType::CaseInsensitive)) {
                return Success<Action>(Action { Type::KeyboardKey, SDL_SCANCODE_PERIOD });
            }
            if (key.equals("slash", StringCompareType::CaseInsensitive)) {
                return Success<Action>(Action { Type::KeyboardKey, SDL_SCANCODE_SLASH });
            }
            if (key.equals("semicolon", StringCompareType::CaseInsensitive)) {
                return Success<Action>(Action { Type::KeyboardKey, SDL_SCANCODE_SEMICOLON });
            }
            if (key.equals_any({ "equal", "equals" }, StringCompareType::CaseInsensitive)) {
                return Success<Action>(Action { Type::KeyboardKey, SDL_SCANCODE_EQUALS });
            }
            if (key.equals("left_bracket", StringCompareType::CaseInsensitive)) {
                return Success<Action>(Action { Type::KeyboardKey, SDL_SCANCODE_LEFTBRACKET });
            }
            if (key.equals("right_bracket", StringCompareType::CaseInsensitive)) {
                return Success<Action>(Action { Type::KeyboardKey, SDL_SCANCODE_RIGHTBRACKET });
            }
            if (key.equals("grave_accent", StringCompareType::CaseInsensitive)) {
                return Success<Action>(Action { Type::KeyboardKey, SDL_SCANCODE_GRAVE });
            }
            if (key.equals("backslash", StringCompareType::CaseInsensitive)) {
                return Success<Action>(Action { Type::KeyboardKey, SDL_SCANCODE_BACKSLASH });
            }
            if (key.equals("escape", StringCompareType::CaseInsensitive)) {
                return Success<Action>(Action { Type::KeyboardKey, SDL_SCANCODE_ESCAPE });
            }
            if (key.equals("tab", StringCompareType::CaseInsensitive)) {
                return Success<Action>(Action { Type::KeyboardKey, SDL_SCANCODE_TAB });
            }
            if (key.equals("backspace", StringCompareType::CaseInsensitive)) {
                return Success<Action>(Action { Type::KeyboardKey, SDL_SCANCODE_BACKSPACE });
            }
            if (key.equals("insert", StringCompareType::CaseInsensitive)) {
                return Success<Action>(Action { Type::KeyboardKey, SDL_SCANCODE_INSERT });
            }
            if (key.equals("delete", StringCompareType::CaseInsensitive)) {
                return Success<Action>(Action { Type::KeyboardKey, SDL_SCANCODE_DELETE });
            }
            if (key.equals("right", StringCompareType::CaseInsensitive)) {
                return Success<Action>(Action { Type::KeyboardKey, SDL_SCANCODE_RIGHT });
            }
            if (key.equals("left", StringCompareType::CaseInsensitive)) {
                return Success<Action>(Action { Type::KeyboardKey, SDL_SCANCODE_LEFT });
            }
            if (key.equals("down", StringCompareType::CaseInsensitive)) {
                return Success<Action>(Action { Type::KeyboardKey, SDL_SCANCODE_DOWN });
            }
            if (key.equals("up", StringCompareType::CaseInsensitive)) {
                return Success<Action>(Action { Type::KeyboardKey, SDL_SCANCODE_UP });
            }
            if (key.equals("page_up", StringCompareType::CaseInsensitive)) {
                return Success<Action>(Action { Type::KeyboardKey, SDL_SCANCODE_PAGEUP });
            }
            if (key.equals("page_down", StringCompareType::CaseInsensitive)) {
                return Success<Action>(Action { Type::KeyboardKey, SDL_SCANCODE_PAGEDOWN });
            }
            if (key.equals("home", StringCompareType::CaseInsensitive)) {
                return Success<Action>(Action { Type::KeyboardKey, SDL_SCANCODE_HOME });
            }
            if (key.equals("end", StringCompareType::CaseInsensitive)) {
                return Success<Action>(Action { Type::KeyboardKey, SDL_SCANCODE_END });
            }
            if (key.equals_any({ "caps_lock", "capslock" }, StringCompareType::CaseInsensitive)) {
                return Success<Action>(Action { Type::KeyboardKey, SDL_SCANCODE_CAPSLOCK });
            }
            if (key.equals_any({ "scroll_lock", "scrolllock" }, StringCompareType::CaseInsensitive)) {
                return Success<Action>(Action { Type::KeyboardKey, SDL_SCANCODE_SCROLLLOCK });
            }
            if (key.equals_any({ "num_lock", "numlock" }, StringCompareType::CaseInsensitive)) {
                return Success<Action>(Action { Type::KeyboardKey, SDL_SCANCODE_NUMLOCKCLEAR });
            }
            if (key.equals("printscreen", StringCompareType::CaseInsensitive)) {
                return Success<Action>(Action { Type::KeyboardKey, SDL_SCANCODE_PRINTSCREEN });
            }
            if (key.equals("pause", StringCompareType::CaseInsensitive)) {
                return Success<Action>(Action { Type::KeyboardKey, SDL_SCANCODE_PAUSE });
            }
            if (key.equals("left_shift", StringCompareType::CaseInsensitive)) {
                return Success<Action>(Action { Type::KeyboardKey, SDL_SCANCODE_LSHIFT });
            }
            if (key.equals("left_ctrl", StringCompareType::CaseInsensitive)) {
                return Success<Action>(Action { Type::KeyboardKey, SDL_SCANCODE_LCTRL });
            }
            if (key.equals("left_alt", StringCompareType::CaseInsensitive)) {
                return Success<Action>(Action { Type::KeyboardKey, SDL_SCANCODE_LALT });
            }
            if (key.equals("right_shift", StringCompareType::CaseInsensitive)) {
                return Success<Action>(Action { Type::KeyboardKey, SDL_SCANCODE_RSHIFT });
            }
            if (key.equals("right_ctrl", StringCompareType::CaseInsensitive)) {
                return Success<Action>(Action { Type::KeyboardKey, SDL_SCANCODE_RCTRL });
            }
            if (key.equals("right_alt", StringCompareType::CaseInsensitive)) {
                return Success<Action>(Action { Type::KeyboardKey, SDL_SCANCODE_RALT });
            }
            if (key.equals("menu", StringCompareType::CaseInsensitive)) {
                return Success<Action>(Action { Type::KeyboardKey, SDL_SCANCODE_MENU });
            }
            return Failure(std::format("Invalid key code \"{}\"", key));
        }
        Result<String> ActionToString(Action action) {
            switch (action.type) {
                case Type::KeyboardKey: {
                    if (action.keycode >= SDL_SCANCODE_0 && action.keycode <= SDL_SCANCODE_9) {
                        return Success<String>(std::to_string(action.keycode - SDL_SCANCODE_0));
                    }
                    if (action.keycode >= SDL_SCANCODE_A && action.keycode <= SDL_SCANCODE_Z) {
                        return Success<String>(String { static_cast<char>(action.keycode - SDL_SCANCODE_A + 'A'), 1 });
                    }
                    if (action.keycode >= SDL_SCANCODE_KP_0 && action.keycode <= SDL_SCANCODE_KP_9) {
                        return Success<String>(std::format("kp_{}", action.keycode - SDL_SCANCODE_KP_0));
                    }
                    if (action.keycode >= SDL_SCANCODE_F1 && action.keycode <= SDL_SCANCODE_F12) {
                        return Success<String>(std::format("F{}", action.keycode - SDL_SCANCODE_F1 + 1));
                    }

                    switch (action.keycode) {
                        case SDL_SCANCODE_SPACE: return Success<String>("space");
                        case SDL_SCANCODE_APOSTROPHE: return Success<String>("apostrophe");
                        case SDL_SCANCODE_COMMA: return Success<String>("comma");
                        case SDL_SCANCODE_MINUS: return Success<String>("minus");
                        case SDL_SCANCODE_PERIOD: return Success<String>("period");
                        case SDL_SCANCODE_SLASH: return Success<String>("slash");
                        case SDL_SCANCODE_SEMICOLON: return Success<String>("semicolon");
                        case SDL_SCANCODE_EQUALS: return Success<String>("equal");
                        case SDL_SCANCODE_LEFTBRACKET: return Success<String>("left_bracket");
                        case SDL_SCANCODE_BACKSLASH: return Success<String>("backslash");
                        case SDL_SCANCODE_RIGHTBRACKET: return Success<String>("right_bracket");
                        case SDL_SCANCODE_GRAVE: return Success<String>("grave_accent");
                        case SDL_SCANCODE_ESCAPE: return Success<String>("escape");
                        case SDL_SCANCODE_RETURN: return Success<String>("enter");
                        case SDL_SCANCODE_TAB: return Success<String>("tab");
                        case SDL_SCANCODE_BACKSPACE: return Success<String>("backspace");
                        case SDL_SCANCODE_INSERT: return Success<String>("insert");
                        case SDL_SCANCODE_DELETE: return Success<String>("delete");
                        case SDL_SCANCODE_RIGHT: return Success<String>("right");
                        case SDL_SCANCODE_LEFT: return Success<String>("left");
                        case SDL_SCANCODE_DOWN: return Success<String>("down");
                        case SDL_SCANCODE_UP: return Success<String>("up");
                        case SDL_SCANCODE_PAGEUP: return Success<String>("page_up");
                        case SDL_SCANCODE_PAGEDOWN: return Success<String>("page_down");
                        case SDL_SCANCODE_HOME: return Success<String>("home");
                        case SDL_SCANCODE_END: return Success<String>("end");
                        case SDL_SCANCODE_CAPSLOCK: return Success<String>("caps_lock");
                        case SDL_SCANCODE_SCROLLLOCK: return Success<String>("scroll_lock");
                        case SDL_SCANCODE_NUMLOCKCLEAR: return Success<String>("num_lock");
                        case SDL_SCANCODE_PRINTSCREEN: return Success<String>("printscreen");
                        case SDL_SCANCODE_PAUSE: return Success<String>("pause");
                        case SDL_SCANCODE_KP_DECIMAL: return Success<String>("kp_decimal");
                        case SDL_SCANCODE_KP_DIVIDE: return Success<String>("kp_divide");
                        case SDL_SCANCODE_KP_MULTIPLY: return Success<String>("kp_multiply");
                        case SDL_SCANCODE_KP_MINUS: return Success<String>("kp_subtract");
                        case SDL_SCANCODE_KP_PLUS: return Success<String>("kp_add");
                        case SDL_SCANCODE_KP_ENTER: return Success<String>("kp_enter");
                        case SDL_SCANCODE_KP_EQUALS: return Success<String>("kp_equal");
                        case SDL_SCANCODE_LSHIFT: return Success<String>("left_shift");
                        case SDL_SCANCODE_LCTRL: return Success<String>("left_ctrl");
                        case SDL_SCANCODE_LALT: return Success<String>("left_alt");
                        case SDL_SCANCODE_RSHIFT: return Success<String>("right_shift");
                        case SDL_SCANCODE_RCTRL: return Success<String>("right_ctrl");
                        case SDL_SCANCODE_RALT: return Success<String>("right_alt");
                        case SDL_SCANCODE_MENU: return Success<String>("menu");
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
            SDL_SetWindowRelativeMouseMode(Engine::s_window, mode == CursorMode::Locked);
        }
        CursorMode GetCursorMode() {
            return SDL_GetWindowRelativeMouseMode(Engine::s_window) ? CursorMode::Locked : CursorMode::Unlocked;
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

        Vector2 MousePosition() {
            return s_mouse_position;
        }
        Vector2 MouseMovement() {
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
        FOW_DISCARD(args);
        Console::ToggleConsoleVisible();
        return Success();
    }

    Result<> SetSceneCommand(const Vector<String>& args) {
        if (args.size() < 1) {
            return Failure("Usage: set_scene <scene asset path>");
        }

        auto scene = Assets::Load<Scene>(args.at(0));
        if (!scene.has_value()) {
            return Failure(scene.error());
        }

        Engine::SetScene(scene.value().ptr());

        return Success();
    }

#ifndef NDEBUG
    static void GLDebugMessageCallback(const GLenum source, const GLenum type, GLuint id, const GLenum severity, const GLsizei length, const GLchar* message, const void* userParam) {
        FOW_DISCARD(length);
        FOW_DISCARD(userParam);

        String source_str;
        switch (source) {
            case GL_DEBUG_SOURCE_API: source_str = "API"; break;
            case GL_DEBUG_SOURCE_WINDOW_SYSTEM: source_str = "Window System"; break;
            case GL_DEBUG_SOURCE_SHADER_COMPILER: source_str = "Shader Compiler"; break;
            case GL_DEBUG_SOURCE_THIRD_PARTY: source_str = "Third Party"; break;
            case GL_DEBUG_SOURCE_APPLICATION: source_str = "Application"; break;
            default: source_str = "Unknown"; break;
        }

        String severity_str;
        switch (severity) {
            case GL_DEBUG_SEVERITY_HIGH: severity_str = "High"; break;
            case GL_DEBUG_SEVERITY_MEDIUM: severity_str = "Medium"; break;
            case GL_DEBUG_SEVERITY_LOW: severity_str = "Low"; break;
            case GL_DEBUG_SEVERITY_NOTIFICATION: severity_str = "Notification"; break;
            default: severity_str = "Unknown"; break;
        }

        LogLevel log_level;
        switch (type) {
            case GL_DEBUG_TYPE_ERROR:
            case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: log_level = LogLevel::Error; break;
            case GL_DEBUG_TYPE_PORTABILITY:
            case GL_DEBUG_TYPE_PERFORMANCE:
            case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: log_level = LogLevel::Warning; break;
            default: log_level = LogLevel::Info; break;
        }

        Debug::Log(log_level, std::format("OpenGL (id 0x{:X}) [Source: {}, Severity: {}] - {}", id, source_str, severity_str, message));
    }
#endif
}
