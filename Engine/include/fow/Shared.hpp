#ifndef FOW_SHARED_HPP
#define FOW_SHARED_HPP

#include <string>
#include <cstdint>
#include <cstring>
#include <chrono>
#include <unordered_map>

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

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

namespace fow {
    namespace Time {
        using namespace std::chrono;
        using TimePoint = system_clock::time_point;
        inline auto Now() {
            return zoned_time(current_zone(), system_clock::now());
        }
    }
    template<typename T>
    T* copy_ptr(T* trg, const T* src, const size_t count) {
        return static_cast<T*>(memcpy(static_cast<void*>(trg), static_cast<const void*>(src), count * sizeof(T)));
    }
    template<typename T>
    T* move_ptr(T* trg, const T* src, const size_t count) {
        return static_cast<T*>(memmove(static_cast<void*>(trg), static_cast<const void*>(src), count * sizeof(T)));
    }
    template<typename T>
    T* resize_ptr(T* ptr, const size_t old_size, const size_t new_size) {
        T* new_ptr = new T[new_size];
        copy_ptr(new_ptr, ptr, std::min(old_size, new_size));
        delete[] ptr;
        return new_ptr;
    }

    template<typename T>
    using Vector = std::vector<T>;
    template<typename K, typename V>
    using HashMap = std::unordered_map<K, V>;

    template<typename T>
    concept IntegerType = std::is_integral_v<T>;
    template<typename T>
    concept FloatType = std::is_floating_point_v<T>;
    template<typename T>
    concept NumberType = std::is_integral_v<T> || std::is_floating_point_v<T>;

    template<typename T>
    using SharedPtr = std::shared_ptr<T>;
    template<typename T>
    using UniquePtr = std::unique_ptr<T>;

    class FOW_SHARED_API Color : public glm::vec4 {
    public:
        constexpr Color() : glm::vec4(0.0f, 0.0f, 0.0f, 1.0f) { }
        constexpr Color(const float r, const float g, const float b, const float a = 1.0f) : glm::vec4(r, g, b, a) { }

        static constexpr Color FromBytes(const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t a = 0xFF) {
            return Color { static_cast<float>(r) / 255.0f, static_cast<float>(g) / 255.0f, static_cast<float>(b) / 255.0f, static_cast<float>(a) / 255.0f };
        }
        static constexpr Color FromInt(const uint32_t rgba) {
            return FromBytes((rgba >> 24) & 0xFF, (rgba >> 16) & 0xFF, (rgba >> 8) & 0xFF, rgba & 0xFF);
        }

        [[nodiscard]] constexpr std::tuple<uint8_t, uint8_t, uint8_t, uint8_t> to_bytes() const {
            return std::make_tuple(
                static_cast<uint8_t>(r * 255.0f),
                static_cast<uint8_t>(g * 255.0f),
                static_cast<uint8_t>(b * 255.0f),
                static_cast<uint8_t>(a * 255.0f)
            );
        }
        [[nodiscard]] constexpr uint32_t to_int() const {
            const auto [ r, g, b, a ] = to_bytes();
            return r << 24 | g << 16 | b << 8 | a;
        }
    };

    enum class ColorFormat {
        RgbaFloat,
        RgbaBytes,
        Hex
    };

    struct FOW_SHARED_API Rectangle {
        float x, y, width, height;

        constexpr Rectangle() : x(0), y(0), width(0), height(0) { }
        constexpr Rectangle(Rectangle&& other) noexcept = default;
        constexpr Rectangle(const Rectangle& other)     = default;
        constexpr Rectangle(const glm::vec2& position, const glm::vec2& size) : x(position.x), y(position.y), width(size.x), height(size.y) { }
        constexpr Rectangle(const float x, const float y, const float width, const float height) : x(x), y(y), width(width), height(height) { }
        constexpr Rectangle(const float x, const float y, const glm::vec2& size) : x(x), y(y), width(size.x), height(size.y) { }
        constexpr Rectangle(const glm::vec2& position, const float width, const float height) : x(position.x), y(position.y), width(width), height(height) { }

        Rectangle& operator=(const Rectangle& other)     = default;
        Rectangle& operator=(Rectangle&& other) noexcept = default;
    };
}

#endif