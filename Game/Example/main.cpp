#include "fow/Core.hpp"

#include "Scenes.hpp"

using namespace fow;

class ExampleGame : public Game {
public:
    ExampleGame() : Game() { }

    void on_init() override {
        SceneExample();
    }
    void on_update(const double dt) override {
    }
    void on_render(const double dt) override {
        Debug::DrawAxis(Vector3Constants::Zero);
    }
    void on_close() override {
    }

    [[nodiscard]] FOW_CONSTEXPR String title() const override {
        return "Example";
    }
    [[nodiscard]] Vector<String> game_data_archives() const override {
        return Vector<String> { "Data.pak" };
    }
    [[nodiscard]] FOW_CONSTEXPR bool allow_mods() const override {
        return FOW_MODS_ENABLED;
    }
    [[nodiscard]] FOW_CONSTEXPR bool editor_enabled() const override {
        return FOW_EDITOR_ENABLED;
    }
    void on_update_imgui(double dt) override {
    }
    [[nodiscard]] FOW_CONSTEXPR Version version() const override {
        return { 0, 1, 0 };
    }
};

FOW_ENTRY_POINT(ExampleGame)