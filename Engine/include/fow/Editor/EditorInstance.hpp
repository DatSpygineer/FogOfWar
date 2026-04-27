#ifndef FOW_EDITOR_INSTANCE_HPP
#define FOW_EDITOR_INSTANCE_HPP

#include "fow/Shared.hpp"

namespace fow {
    FOW_EDITOR_API void EditorInit();
    FOW_EDITOR_API void EditorTerminate();
    FOW_EDITOR_API void EditorTick();
    FOW_ENGINE_API bool EditorIsRunning();
}

#endif