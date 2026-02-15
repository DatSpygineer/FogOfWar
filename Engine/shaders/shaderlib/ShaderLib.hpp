#ifndef SHADERCACHE_HPP
#define SHADERCACHE_HPP

#include <string>
#include <optional>
#include <memory>
#include <unordered_map>

#ifdef SHADER_LIB_EXPORTS
    #ifdef _WIN32
        #define SHADER_LIB_API __declspec(dllexport)
    #else
        #define SHADER_LIB_API __attribute__((visibility("default")))
    #endif
#else
    #ifdef _WIN32
        #define SHADER_LIB_API __declspec(dllimport)
    #else
        #define SHADER_LIB_API
    #endif
#endif

extern "C" {
    struct SHADER_LIB_API ShaderSources {
        std::string vertex, fragment;
    };

    SHADER_LIB_API void ShaderLibInitialize();
    SHADER_LIB_API const char* ShaderLibGetSource(const char* name);
    SHADER_LIB_API bool ShaderLibGetRequiredSources(const char* name, ShaderSources* sources);
}

#endif