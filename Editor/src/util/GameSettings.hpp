#ifndef FOW_EDITOR_GAMESETTINGS_HPP
#define FOW_EDITOR_GAMESETTINGS_HPP

#include <fow/Shared.hpp>

namespace fow {
    struct GameSettings {
        Path GamePath;
        Vector<String> GameDataPacks;
        Option<Path> ModPath;
    };

    GameSettings& InitializeGameSettings(const HashMap<String, Vector<String>>& args);
    GameSettings& GetGameSettings();
}

#endif