#include "fow/Core.hpp"

#include "Scenes.hpp"

using namespace fow;

class ExampleGame : public Game {
public:
    ExampleGame() : Game() { }

    void on_init() override {
        Scene3DExample();
    }
    void on_update(const double dt) override {
    }
    void on_render(const double dt) override {
    }
    void on_close() override {
    }

    [[nodiscard]] FOW_CONSTEXPR String title() const override {
        return "Example";
    }
    [[nodiscard]] Vector<String> game_data_archives() const override {
        return Vector<String> { "Data.pak" };
    }
    [[nodiscard]] bool allow_mods() const override {
        return FOW_MODS_ENABLED;
    }
    void on_update_imgui(double dt) override {
    }
};

FOW_ENTRY_POINT(ExampleGame)