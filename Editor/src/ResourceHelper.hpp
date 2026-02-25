#ifndef FOW_EDITOR_RESOURCE_HELPER_HPP
#define FOW_EDITOR_RESOURCE_HELPER_HPP

#include <fow/Shared.hpp>

#include <QIcon>

extern fow::Path EditorBaseDir;

namespace fow {
    inline QIcon LoadEditorIcon(const Path& path) {
        return QIcon((EditorBaseDir / path).as_cstr());
    }
}

#endif