#ifndef FOW_ENGINE_HPP
#define FOW_ENGINE_HPP

#include <functional>

#include "fow/Shared.hpp"
#include "fow/String.hpp"
#include "fow/Result.hpp"
#include "fow/Version.hpp"
#include "fow/KeyCodeWrapper.hpp"

namespace fow {
    enum class WindowMode {
        Windowed,
        Fullscreen
    };

    class FOW_ENGINE_API Game {
    public:
        virtual ~Game() = default;

        ABSTRACT(void on_init());
        ABSTRACT(void on_update(double dt));
        ABSTRACT(void on_render(double dt));
        ABSTRACT(void on_close());
        ABSTRACT(String title() const);
        ABSTRACT(Vector<String> game_data_archives() const);
        ABSTRACT(Path base_data_path() const);
        virtual Option<Path> mod_data_path() const { return None(); }
        virtual void on_window_resized(const glm::ivec2& new_size) { }
        virtual void on_update_imgui(double dt) { }
    };

    namespace Engine {
        FOW_ENGINE_API Path GetGameBasePath();
        FOW_ENGINE_API Path GetResourcesPath();
        FOW_ENGINE_API void SetBackgroundColor(const Color& color);
        FOW_ENGINE_API void SetBackgroundColor(Color&& color) noexcept;
        FOW_ENGINE_API Color GetBackgroundColor();
        FOW_ENGINE_API Result<> Initialize(int argc, char** argv, const String& title, const std::function<std::shared_ptr<Game>()>& game_class_ctor);
        FOW_ENGINE_API void Run();
        FOW_ENGINE_API void SetWindowTitle(const String& title);
        FOW_ENGINE_API void SetWindowPosition(const glm::ivec2& value);
        FOW_ENGINE_API void SetWindowSize(const glm::ivec2& value);
        FOW_ENGINE_API String GetWindowTitle();
        FOW_ENGINE_API glm::ivec2 GetWindowPosition();
        FOW_ENGINE_API glm::ivec2 GetWindowSize();
        FOW_ENGINE_API const Version& GetVersion();
    }

    namespace Input {
        enum class Type {
            KeyboardKey,
            MouseButton,
            MouseMotion,
            MouseWheel
        };
        enum class State {
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

        FOW_ENGINE_API void Initialize();
        FOW_ENGINE_API void Poll();

        FOW_ENGINE_API Result<> CreateAction(const String& name, Type type, int value);
        inline Result<> CreateAction(const String& name, const Type type, const KeyCode value) { return CreateAction(name, type, static_cast<int>(value)); }

        FOW_ENGINE_API bool ActionIsPressed(const String& action);
        FOW_ENGINE_API bool ActionIsDown(const String& action);
        FOW_ENGINE_API bool ActionIsReleased(const String& action);
        FOW_ENGINE_API bool ActionIsUp(const String& action);

        FOW_ENGINE_API float GetAxis(const String& positive_action, const String& negative_action, float multiplier = 1.0f);
        FOW_ENGINE_API float GetAxis(const String& action, float multiplier = 1.0f);
        inline glm::vec2 GetAxis2D(
            const String& x_positive, const String& x_negative,
            const String& y_positive, const String& y_negative,
            const glm::vec2& multiplier = glm::vec2 { 1.0f }
        ) {
            return glm::vec2 {
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

        FOW_ENGINE_API glm::vec2 MousePosition();
        FOW_ENGINE_API glm::vec2 MouseMovement();
    }
}

#endif