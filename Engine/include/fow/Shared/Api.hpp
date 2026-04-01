#ifndef FOW_API_HPP
#define FOW_API_HPP

#ifdef __cplusplus
    #include <cstdint>
#else
    #include <stdint.h>
#endif

#if defined(_MSC_VER) && !defined(__clang__)
    #define FOW_CONSTEXPR_ENABLED 0
#else
    #define FOW_CONSTEXPR_ENABLED 1
#endif

#if FOW_CONSTEXPR_ENABLED
    #define FOW_CONSTEXPR constexpr
#else
    #define FOW_CONSTEXPR inline
#endif

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
        typedef int64_t ssize_t;
    #else
        typedef int32_t ssize_t;
    #endif
#endif

#ifdef __cplusplus
    #define FOW_ABSTRACT(__x) virtual __x = 0;
#endif

#define FOW_DISCARD(__x) ((void)(__x))

#define FOW_CONCAT(__a, __b) __a##__b
#define FOW_CONCAT_2(__a, __b) FOW_CONCAT(__a, __b)

#define FOW_NAME_WITH_LINE_NUMBER(__prefix) FOW_CONCAT_2(__prefix, __LINE__)
#define FOW_UNIQUE(__prefix) FOW_CONCAT_2(__prefix, __COUNTER__)

#endif