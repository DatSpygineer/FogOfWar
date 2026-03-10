#ifndef FOW_EDITOR_RESOURCE_HELPER_HPP
#define FOW_EDITOR_RESOURCE_HELPER_HPP

#include <fow/Shared.hpp>

#include <QIcon>

extern fow::Path EditorBaseDir;

namespace fow {
    inline QIcon LoadEditorIcon(const Path& path) {
        return QIcon((EditorBaseDir / path).as_cstr());
    }
    inline Result<pugi::xml_document> LoadEditorMappingXml(const String& name) {
        auto path = EditorBaseDir / "Editor" / "mappings" / name;
        path.replace_extension("xml");
        pugi::xml_document doc;
        if (const auto result = doc.load_file(path.as_cstr()); result.status != pugi::status_ok) {
            return Failure(std::format("Failed to load xml document \"{}\": {}", path, result.description()));
        }
        return Success<pugi::xml_document>(std::move(doc));
    }
    inline Result<pugi::xml_document> LoadEditorShaderParamsXml(const String& name) {
        auto path = EditorBaseDir / "Editor" / "shaderparams" / name;
        path.replace_extension("xml");
        pugi::xml_document doc;
        if (const auto result = doc.load_file(path.as_cstr()); result.status != pugi::status_ok) {
            return Failure(std::format("Failed to load xml document \"{}\": {}", path, result.description()));
        }
        return Success<pugi::xml_document>(std::move(doc));
    }
}

#endif