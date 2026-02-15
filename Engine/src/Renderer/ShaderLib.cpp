#include "fow/Renderer/ShaderLib.hpp"
#include "fow/Shared/Dylib.hpp"

#include "fow/Renderer/GL.hpp"

#ifdef _WIN32
    #define SHADERLIB_FILENAME "shaderlib.dll"
#else
    #define SHADERLIB_FILENAME "libshaderlib.so"
#endif

const fow::Dylib* s_shaderlib = nullptr;

namespace fow::ShaderLib {
    Result<> Load(const Path& base_path) {
        s_shaderlib = new Dylib(base_path / SHADERLIB_FILENAME);
        if (!s_shaderlib->is_valid()) {
            return Failure("Failed to load shader library!");
        }

        if (const auto shaderlib_init = s_shaderlib->symbol<void(*)()>("ShaderLibInitialize"); shaderlib_init != nullptr) {
            shaderlib_init();
            return Success();
        }
        return Failure("Failed to initialize shader library!");
    }

    Result<ShaderSources> GetSourcesForShader(const String& name) {
        if (s_shaderlib == nullptr) {
            return Failure(std::format("Failed to get sources for shader \"{}\": Shader library is not loaded!", name));
        }
        if (const auto fn = s_shaderlib->symbol("ShaderLibGetRequiredSources"); fn != nullptr) {
            ShaderSources sources;
            if (reinterpret_cast<bool(*)(const char*, ShaderSources*)>(fn)(name.as_cstr(), &sources)) {
                return Success<ShaderSources>(sources);
            }
            return Failure(std::format("Failed to get sources for shader \"{}\": No such shader!", name));
        }
        return Failure(std::format("Failed to get sources for shader \"{}\": Could not load function \"ShaderLibGetRequiredSources\"", name));
    }
    Result<String> GetSource(const String& name) {
        if (s_shaderlib == nullptr) {
            return Failure(std::format("Failed to get shader source \"{}\": Shader library is not loaded!", name));
        }
        if (const auto fn = s_shaderlib->symbol("ShaderLibGetSource"); fn != nullptr) {
            const char* src = reinterpret_cast<const char*(*)(const char*)>(fn)(name.as_cstr());
            if (src == nullptr) {
                return Failure(std::format("Failed to get shader source \"{}\": Shader source doesn't exists!", name));
            }
            return Success<String>(src);
        }
        return Failure(std::format("Failed to get shader source \"{}\": Could not load function \"ShaderLibGetSource\"", name));
    }

    void Unload() {
        if (s_shaderlib != nullptr) {
            delete s_shaderlib;
        }
    }
}
