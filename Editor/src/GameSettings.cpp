#include "GameSettings.hpp"

#include <QMessageBox>

#include "rfl/json.hpp"

namespace fow {
    static GameSettings s_settings;

    struct GameSettingsInternal {
        std::vector<std::string> GameDataPacks;
        bool AllowMods;
    };

    GameSettings& InitializeGameSettings(const HashMap<String, Vector<String>>& args) {
        if (args.contains("-game")) {
            const auto game_path = args.at("-game");
            if (game_path.size() > 0) {
                Path::CurrentDir(game_path.at(0));
            }
        }

        s_settings.GamePath = Path::CurrentDir();
        if (const auto settings_path = s_settings.GamePath / ".editor"; settings_path.exists()) {
            std::ifstream ifs(settings_path.as_std_path());
            const auto result = rfl::json::read<GameSettingsInternal>(ifs);
            if (!result.has_value()) {
                QMessageBox::critical(nullptr, "Error", std::format("Failed to parse editor.json: {}", result.error().what()).c_str());
            }

            for (const auto& pack : result->GameDataPacks) {
                s_settings.GameDataPacks.emplace_back(pack);
            }
            s_settings.ModPath = result->AllowMods ? Some(s_settings.GamePath / "mods") : None();
        }
        return s_settings;
    }
    GameSettings& GetGameSettings() {
        return s_settings;
    }
}
