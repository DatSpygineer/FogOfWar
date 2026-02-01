#ifndef FOW_SHADERLIB_HPP
#define FOW_SHADERLIB_HPP

#include "fow/Shared/Api.hpp"
#include "fow/Shared/String.hpp"
#include "fow/Shared/Result.hpp"

namespace fow {
    struct FOW_RENDER_API ShaderSources {
        std::string vertex, fragment;
    };

    namespace ShaderLib {
        FOW_RENDER_API Result<> Load(const Path& base_path);
        FOW_RENDER_API Result<ShaderSources> GetSourcesForShader(const String& name);
        FOW_RENDER_API Result<String> GetSource(const String& name);
        FOW_RENDER_API void Unload();
    }
}

#endif