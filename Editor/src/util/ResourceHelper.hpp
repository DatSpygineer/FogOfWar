#ifndef FOW_EDITOR_RESOURCE_HELPER_HPP
#define FOW_EDITOR_RESOURCE_HELPER_HPP

#include <fow/Shared.hpp>
#include <fow/Renderer.hpp>

#include <QIcon>

extern fow::Path EditorBaseDir;

namespace fow {
    constexpr const char* EditorResourceFolderName() { return "EditorResources"; }
    constexpr const char* EditorProjectFolderName() { return "EditorProjects"; }

    inline Path GetEditorResourcePath(const Path& path) {
        return EditorBaseDir / EditorResourceFolderName() / path;
    }

    inline QIcon LoadEditorIcon(const Path& path) {
        return QIcon(GetEditorResourcePath(path).as_cstr());
    }
    inline ModelPtr LoadEditorModel(const Path& path, const Vector<MaterialPtr>& materials) {
        if (const auto data = Files::ReadAllBytes(GetEditorResourcePath(path)); data.has_value()) {
            return Model::Load(path.as_string(), data.value(), materials).value_or(nullptr);
        }
        return nullptr;
    }
    inline Result<pugi::xml_document> LoadEditorMappingXml(const String& name) {
        auto path = GetEditorResourcePath("mappings"_p / name);
        path.replace_extension("xml");
        pugi::xml_document doc;
        if (const auto result = doc.load_file(path.as_cstr()); result.status != pugi::status_ok) {
            return Failure(std::format("Failed to load xml document \"{}\": {}", path, result.description()));
        }
        return Success<pugi::xml_document>(std::move(doc));
    }
    inline Result<pugi::xml_document> LoadEditorShaderParamsXml(const String& name) {
        auto path = GetEditorResourcePath("shaderparams"_p / name);
        path.replace_extension("xml");
        pugi::xml_document doc;
        if (const auto result = doc.load_file(path.as_cstr()); result.status != pugi::status_ok) {
            return Failure(std::format("Failed to load xml document \"{}\": {}", path, result.description()));
        }
        return Success<pugi::xml_document>(std::move(doc));
    }
}

#endif