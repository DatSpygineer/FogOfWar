#ifndef FOW_ENGINE_HPP
#define FOW_ENGINE_HPP

#include <functional>

#include "fow/Shared.hpp"
#include "fow/Shared/GameState.hpp"
#include "fow/Engine/Convar.hpp"
#include "fow/Engine/KeyCodeWrapper.hpp"
#include "fow/Engine/Entity.hpp"
#include "fow/Engine/Components.hpp"

#ifdef FOW_INCLUDE_IMGUI
    #include "fow/Engine/ImGui.hpp"
    #undef FOW_INCLUDE_IMGUI
#endif

#define __FOW_SHARED_ENTRY_POINT(__game_class) \
    ::fow::Debug::AssertFatal(::fow::Engine::Initialize(argc, argv, []() -> std::shared_ptr<::fow::Game> { \
        return CreateRef<__game_class>(); \
    })); \
    ::fow::Engine::Run(); \

#ifdef _WIN32
    #define FOW_ENTRY_POINT(__game_class) \
    int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) { \
        int argc; \
        LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc); \
        __FOW_SHARED_ENTRY_POINT(__game_class) \
        LocalFree(argv); \
        return 0; \
    }
#else
    #define FOW_ENTRY_POINT(__game_class) \
    int main(const int argc, char** argv) { \
        __FOW_SHARED_ENTRY_POINT(__game_class) \
        return 0; \
    }
#endif

namespace fow {
    enum class WindowMode {
        Windowed,
        Fullscreen
    };

    class FOW_ENGINE_API Game {
    public:
        virtual ~Game() = default;

        FOW_ABSTRACT(void on_init());
        FOW_ABSTRACT(void on_update(double dt));
        FOW_ABSTRACT(void on_render(double dt));
        FOW_ABSTRACT(void on_close());
        FOW_ABSTRACT(String title() const);
        FOW_ABSTRACT(Vector<String> game_data_archives() const);
        FOW_ABSTRACT(bool allow_mods() const);
        virtual void on_window_resized(const Vector2i& new_size) { }
        virtual void on_update_imgui(double dt) { }
    };

    namespace Engine {
        FOW_ENGINE_API Path GetGameBasePath();
        FOW_ENGINE_API Path GetResourcesPath();
        FOW_ENGINE_API void SetBackgroundColor(const Color& color);
        FOW_ENGINE_API void SetBackgroundColor(Color&& color) noexcept;
        FOW_ENGINE_API Color GetBackgroundColor();
        FOW_ENGINE_API Result<> Initialize(int argc, os_char_t** argv, const Function<std::shared_ptr<Game>()>& game_class_ctor);
        FOW_ENGINE_API void Run();
        FOW_ENGINE_API void SetWindowTitle(const String& title);
        FOW_ENGINE_API void SetWindowPosition(const Vector2i& value);
        FOW_ENGINE_API void SetWindowSize(const Vector2i& value);
        FOW_ENGINE_API String GetWindowTitle();
        FOW_ENGINE_API Vector2i GetWindowPosition();
        FOW_ENGINE_API Vector2i GetWindowSize();
        FOW_ENGINE_API const Version& GetVersion();
        FOW_ENGINE_API void SetScene(const ScenePtr& scene);
        FOW_ENGINE_API double Time();
    }

    namespace Input {
        enum class Type {
            KeyboardKey,
            MouseButton,
            MouseMotion,
            MouseWheel
        };
        enum class KeyState {
            Pressed,
            Down,
            Released,
            Up
        };
        enum class MouseButton {
            Left,
            Middle,
            Right,
            Mb5, Mb6, Mb7, Mb8
        };

        struct FOW_ENGINE_API Action {
            Type type;
            int keycode;
        };

        enum class CursorMode {
            Unlocked,
            Locked
        };

        FOW_ENGINE_API void Initialize();
        FOW_ENGINE_API void Poll();

        FOW_ENGINE_API Result<> CreateAction(const String& name, Action action);
        FOW_ENGINE_API Result<> CreateAction(const String& name, Type type, int value);
        inline Result<> CreateAction(const String& name, const Type type, const KeyCode value) { return CreateAction(name, type, static_cast<int>(value)); }
        FOW_ENGINE_API void DeleteAction(const String& name);
        FOW_ENGINE_API const HashMap<String, Action>& AvailableActions();

        FOW_ENGINE_API Result<Action> StringToAction(const String& key);
        FOW_ENGINE_API Result<String> ActionToString(Action action);

        FOW_ENGINE_API void SetCursorMode(CursorMode mode);
        FOW_ENGINE_API CursorMode GetCursorMode();

        FOW_ENGINE_API bool ActionIsPressed(const String& action);
        FOW_ENGINE_API bool ActionIsDown(const String& action);
        FOW_ENGINE_API bool ActionIsReleased(const String& action);
        FOW_ENGINE_API bool ActionIsUp(const String& action);

        FOW_ENGINE_API float GetAxis(const String& positive_action, const String& negative_action, float multiplier = 1.0f);
        FOW_ENGINE_API float GetAxis(const String& action, float multiplier = 1.0f);
        inline Vector2 GetAxis2D(
            const String& x_positive, const String& x_negative,
            const String& y_positive, const String& y_negative,
            const Vector2& multiplier = Vector2 { 1.0f }
        ) {
            return Vector2 {
                GetAxis(x_positive, x_negative, multiplier.x),
                GetAxis(y_positive, y_negative, multiplier.y)
            };
        }

        FOW_ENGINE_API bool KeyIsPressed(KeyCode key);
        FOW_ENGINE_API bool KeyIsDown(KeyCode key);
        FOW_ENGINE_API bool KeyIsReleased(KeyCode key);
        FOW_ENGINE_API bool KeyIsUp(KeyCode key);

        FOW_ENGINE_API bool MouseIsPressed(MouseButton button);
        FOW_ENGINE_API bool MouseIsDown(MouseButton button);
        FOW_ENGINE_API bool MouseIsReleased(MouseButton button);
        FOW_ENGINE_API bool MouseIsUp(MouseButton button);

        FOW_ENGINE_API Vector2 MousePosition();
        FOW_ENGINE_API Vector2 MouseMovement();
    }
}

#endif