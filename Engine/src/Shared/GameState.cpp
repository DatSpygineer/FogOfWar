#include "fow/GameState.hpp"

namespace fow {
    static int s_exit_code = 0;
    static auto s_game_state = GameState::Uninitialized;

    void SetGameStateRunning() {
        if (s_game_state == GameState::Uninitialized || s_game_state == GameState::Closed) {
            s_game_state = GameState::Running;
        }
    }
    void SetGameStateClosing() {
        if (s_game_state < GameState::Closing) {
            s_game_state = GameState::Closing;
        }
    }
    void CrashGame(const int exit_code) {
        s_exit_code = exit_code;
        s_game_state = GameState::Crashed;
    }

    void SetGameStateTerminated() {
        if (s_game_state != GameState::Crashed) {
            s_game_state = GameState::Closed;
        }
    }

    GameState GetGameState() {
        return s_game_state;
    }
    int GetGameExitCode() {
        return s_exit_code;
    }
}