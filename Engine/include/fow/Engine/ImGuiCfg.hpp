#ifndef FOW_IMGUI_CFG_HPP
#define FOW_IMGUI_CFG_HPP

#ifdef imgui_EXPORTS
    #ifdef _WIN32
        #define IMGUI_API __declspec(dllexport)
    #else
        #define IMGUI_API __attribute__((visibility("default")))
    #endif
#else
    #ifdef _WIN32
        #define IMGUI_API __declspec(dllimport)
    #else
        #define IMGUI_API
    #endif
#endif

#endif