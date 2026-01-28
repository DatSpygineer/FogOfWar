#ifndef FOW_API_HPP
#define FOW_API_HPP

#ifdef FogOfWarShared_EXPORTS
    #ifdef _WIN32
        #define FOW_SHARED_API __declspec(dllexport)
    #else
        #define FOW_SHARED_API __attribute__((visibility("default")))
    #endif
#else
    #ifdef _WIN32
        #define FOW_SHARED_API __declspec(dllimport)
    #else
        #define FOW_SHARED_API
    #endif
#endif

#ifdef FogOfWarRenderer_EXPORTS
    #ifdef _WIN32
        #define FOW_RENDER_API __declspec(dllexport)
    #else
        #define FOW_RENDER_API __attribute__((visibility("default")))
    #endif
#else
    #ifdef _WIN32
        #define FOW_RENDER_API __declspec(dllimport)
    #else
        #define FOW_RENDER_API
    #endif
#endif

#ifdef FogOfWarEngine_EXPORTS
    #ifdef _WIN32
        #define FOW_ENGINE_API __declspec(dllexport)
    #else
        #define FOW_ENGINE_API __attribute__((visibility("default")))
    #endif
#else
    #ifdef _WIN32
        #define FOW_ENGINE_API __declspec(dllimport)
    #else
        #define FOW_ENGINE_API
    #endif
#endif

#ifndef __GNUC__
    #ifdef _WIN64
        using ssize_t = int64_t;
    #else
        using ssize_t = int32_t;
    #endif
#endif

#define ABSTRACT(__x) virtual __x = 0;
#define DISCARD(__x) ((void)(__x))


#endif