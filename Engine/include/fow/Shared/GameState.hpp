#ifndef FOW_GAMESTATE_HPP
#define FOW_GAMESTATE_HPP

#include "fow/Shared.hpp"

namespace fow {
    enum class GameState {
        Uninitialized,
        Running,
        Closing,
        Closed,
        Crashed
    };

    FOW_SHARED_API void SetGameStateRunning();
    FOW_SHARED_API void SetGameStateClosing();
    FOW_SHARED_API void CrashGame(int exit_code);
    FOW_SHARED_API void SetGameStateTerminated();
    FOW_SHARED_API GameState GetGameState();
    FOW_SHARED_API int GetGameExitCode();
}

#endif