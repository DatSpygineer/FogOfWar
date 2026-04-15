#ifndef FOW_MATH_HELPER_HPP
#define FOW_MATH_HELPER_HPP

#include "fow/Shared/Api.hpp"

#include <type_traits>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace fow {
    template<typename T>
    concept IntegerType = std::is_integral_v<T>;
    template<typename T>
    concept FloatType = std::is_floating_point_v<T>;
    template<typename T>
    concept NumberType = std::is_integral_v<T> || std::is_floating_point_v<T>;

    using Vector2 = glm::vec2;
    using Vector3 = glm::vec3;
    using Vector4 = glm::vec4;
    using Matrix4 = glm::mat4;
    using Quat    = glm::quat;

    using Vector2b = glm::bvec2;
    using Vector3b = glm::bvec3;
    using Vector4b = glm::bvec4;

    using Vector2i = glm::ivec2;
    using Vector3i = glm::ivec3;
    using Vector4i = glm::ivec4;

    using Vector2u = glm::uvec2;
    using Vector3u = glm::uvec3;
    using Vector4u = glm::uvec4;

    template<FloatType T>
    FOW_CONSTEXPR T Lerp(const T a, const T b, const T t) {
        return a + (b - a) * t;
    }
    FOW_CONSTEXPR Vector2 Lerp(const Vector2 a, const Vector2 b, const float t) {
        return a + (b - a) * t;
    }
    FOW_CONSTEXPR Vector3 Lerp(const Vector3 a, const Vector3 b, const float t) {
        return a + (b - a) * t;
    }
    FOW_CONSTEXPR Vector4 Lerp(const Vector4 a, const Vector4 b, const float t) {
        return a + (b - a) * t;
    }
    template<FloatType T>
    FOW_CONSTEXPR T Normalize(const T value, const T min, const T max) {
        return (value - min) / (max - min);
    }
    template<FloatType T>
    FOW_CONSTEXPR T Remap(const T value, const T from_min, const T from_max, const T to_min, const T to_max) {
        return Lerp(to_min, to_max, Normalize(value, from_min, from_max));
    }
    template<FloatType T>
    FOW_CONSTEXPR T Min(const T a, const T b) {
        return a < b ? a : b;
    }
    template<FloatType T>
    FOW_CONSTEXPR T Max(const T a, const T b) {
        return a > b ? a : b;
    }
    template<FloatType T>
    FOW_CONSTEXPR T Clamp(const T value, const T min, const T max) {
        return Min(Max(value, min), max);
    }

    struct FOW_SHARED_API Color : public Vector4 {
        FOW_CONSTEXPR Color() : Vector4(0.0f, 0.0f, 0.0f, 1.0f) { }
        FOW_CONSTEXPR explicit Color(const float value) : Vector4(value, value, value, 1.0f) { }
        FOW_CONSTEXPR Color(const float r, const float g, const float b, const float a = 1.0f) : Vector4(r, g, b, a) { }

        static FOW_CONSTEXPR Color FromBytes(const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t a = 0xFF) {
            return Color { static_cast<float>(r) / 255.0f, static_cast<float>(g) / 255.0f, static_cast<float>(b) / 255.0f, static_cast<float>(a) / 255.0f };
        }
        static FOW_CONSTEXPR Color FromInt(const uint32_t rgba) {
            return FromBytes((rgba >> 24) & 0xFF, (rgba >> 16) & 0xFF, (rgba >> 8) & 0xFF, rgba & 0xFF);
        }

        [[nodiscard]] FOW_CONSTEXPR std::tuple<uint8_t, uint8_t, uint8_t, uint8_t> to_bytes() const {
            return std::make_tuple(
                static_cast<uint8_t>(r * 255.0f),
                static_cast<uint8_t>(g * 255.0f),
                static_cast<uint8_t>(b * 255.0f),
                static_cast<uint8_t>(a * 255.0f)
            );
        }
        [[nodiscard]] FOW_CONSTEXPR uint32_t to_int() const {
            const auto [ r, g, b, a ] = to_bytes();
            return r << 24 | g << 16 | b << 8 | a;
        }

        [[nodiscard]] FOW_CONSTEXPR Vector3 to_vec3() const {
            return Vector3 { r, g, b };
        }
        [[nodiscard]] FOW_CONSTEXPR Vector4 to_vec4() const {
            return Vector4 { r, g, b, a };
        }
    };

    enum class ColorFormat {
        RgbaFloat,
        RgbaBytes,
        Hex
    };

    struct FOW_SHARED_API Rectangle {
        float x, y, width, height;

        FOW_CONSTEXPR Rectangle() : x(0), y(0), width(0), height(0) { }
        FOW_CONSTEXPR Rectangle(Rectangle&& other) noexcept = default;
        FOW_CONSTEXPR Rectangle(const Rectangle& other)     = default;
        FOW_CONSTEXPR Rectangle(const Vector2& position, const Vector2& size) : x(position.x), y(position.y), width(size.x), height(size.y) { }
        FOW_CONSTEXPR Rectangle(const float x, const float y, const float width, const float height) : x(x), y(y), width(width), height(height) { }
        FOW_CONSTEXPR Rectangle(const float x, const float y, const Vector2& size) : x(x), y(y), width(size.x), height(size.y) { }
        FOW_CONSTEXPR Rectangle(const Vector2& position, const float width, const float height) : x(position.x), y(position.y), width(width), height(height) { }

        Rectangle& operator=(const Rectangle& other)     = default;
        Rectangle& operator=(Rectangle&& other) noexcept = default;

        [[nodiscard]] FOW_CONSTEXPR bool is_point_inside(const Vector2& point) const {
            return point.x >= x && point.x < x + width && point.y >= y && point.y < y + height;
        }
        [[nodiscard]] FOW_CONSTEXPR bool intersects(const Rectangle& rect) const;

        [[nodiscard]] FOW_CONSTEXPR Vector2 position() const { return { x, y }; }
        [[nodiscard]] FOW_CONSTEXPR Vector2 size() const { return { width, height }; }
    };

    struct FOW_SHARED_API IntRectangle {
        int x, y, width, height;

        FOW_CONSTEXPR IntRectangle() : x(0), y(0), width(0), height(0) { }
        FOW_CONSTEXPR IntRectangle(IntRectangle&& other) noexcept = default;
        FOW_CONSTEXPR IntRectangle(const IntRectangle& other)     = default;
        FOW_CONSTEXPR IntRectangle(const Vector2i& position, const Vector2i& size) : x(position.x), y(position.y), width(size.x), height(size.y) { }
        FOW_CONSTEXPR IntRectangle(const int x, const int y, const int width, const int height) : x(x), y(y), width(width), height(height) { }
        FOW_CONSTEXPR IntRectangle(const int x, const int y, const Vector2i& size) : x(x), y(y), width(size.x), height(size.y) { }
        FOW_CONSTEXPR IntRectangle(const Vector2i& position, const int width, const int height) : x(position.x), y(position.y), width(width), height(height) { }
        FOW_CONSTEXPR explicit IntRectangle(const Rectangle& other) :
            x(static_cast<int>(other.x)), y(static_cast<int>(other.y)),
            width(static_cast<int>(other.width)), height(static_cast<int>(other.height)) { }

        IntRectangle& operator=(const IntRectangle& other)     = default;
        IntRectangle& operator=(IntRectangle&& other) noexcept = default;

        [[nodiscard]] FOW_CONSTEXPR bool is_point_inside(const Vector2i& point) const {
            return point.x >= x && point.x < x + width && point.y >= y && point.y < y + height;
        }
        [[nodiscard]] FOW_CONSTEXPR bool intersects(const IntRectangle& rect) const;

        [[nodiscard]] FOW_CONSTEXPR Vector2i position() const { return { x, y }; }
        [[nodiscard]] FOW_CONSTEXPR Vector2i size() const { return { width, height }; }

        [[nodiscard]] FOW_CONSTEXPR operator Rectangle() const { return Rectangle {
                static_cast<float>(x), static_cast<float>(y),
                static_cast<float>(width), static_cast<float>(height)
            };
        }
    };

    namespace Vector2iConstants {
#if !FOW_CONSTEXPR_ENABLED
        const Vector2i Up;
        const Vector2i Down;
        const Vector2i Left;
        const Vector2i Right;
        const Vector2i Zero;
        const Vector2i One;

        const Vector2i UnitX;
        const Vector2i UnitY;
#else
        constexpr Vector2i Up    = {  0,  1 };
        constexpr Vector2i Down  = {  0, -1 };
        constexpr Vector2i Left  = { -1,  0 };
        constexpr Vector2i Right = {  1,  0 };
        constexpr Vector2i Zero  = {  0,  0 };
        constexpr Vector2i One   = {  1,  1 };

        constexpr Vector2i UnitX = { 1, 0 };
        constexpr Vector2i UnitY = { 0, 1 };
#endif
    }

    namespace Vector2Constants {
#if !FOW_CONSTEXPR_ENABLED
        const Vector2 Up;
        const Vector2 Down;
        const Vector2 Left;
        const Vector2 Right;
        const Vector2 Zero;
        const Vector2 One;

        const Vector2 UnitX;
        const Vector2 UnitY;
#else
        constexpr Vector2 Up    = { 0.0f, 1.0f };
        constexpr Vector2 Down  = { 0.0f, -1.0f };
        constexpr Vector2 Left  = { -1.0f, 0.0f };
        constexpr Vector2 Right = { 1.0f, 0.0f };
        constexpr Vector2 Zero  = { 0.0f, 0.0f };
        constexpr Vector2 One   = { 1.0f, 1.0f };

        constexpr Vector2 UnitX = { 1.0f, 0.0f };
        constexpr Vector2 UnitY = { 0.0f, 1.0f };
#endif
    }

    namespace Vector3Constants {
#if !FOW_CONSTEXPR_ENABLED
        const Vector3 Up;
        const Vector3 Down;
        const Vector3 Forward;
        const Vector3 Backward;
        const Vector3 Right;
        const Vector3 Left;
        const Vector3 Zero;
        const Vector3 One;

        const Vector3 UnitX;
        const Vector3 UnitY;
        const Vector3 UnitZ;
#else
        constexpr Vector3 Up       = { 0.0f, 1.0f, 0.0f };
        constexpr Vector3 Down     = { 0.0f, -1.0f, 0.0f };
        constexpr Vector3 Forward  = { 0.0f, 0.0f, -1.0f };
        constexpr Vector3 Backward = { 0.0f, 0.0f, 1.0f };
        constexpr Vector3 Right    = { 1.0f, 0.0f, 0.0f };
        constexpr Vector3 Left     = { -1.0f, 0.0f, 0.0f };
        constexpr Vector3 Zero     = { 0.0f, 0.0f, 0.0f };
        constexpr Vector3 One      = { 1.0f, 1.0f, 1.0f };

        constexpr Vector3 UnitX   = { 1.0f, 0.0f, 0.0f };
        constexpr Vector3 UnitY   = { 0.0f, 1.0f, 0.0f };
        constexpr Vector3 UnitZ   = { 0.0f, 0.0f, 1.0f };
#endif
    }

    namespace Vector4Constants {
#if !FOW_CONSTEXPR_ENABLED
        const Vector4 Zero;
        const Vector4 One;

        const Vector4 UnitX;
        const Vector4 UnitY;
        const Vector4 UnitZ;
        const Vector4 UnitW;
#else
        constexpr Vector4 Zero = { 0.0f, 0.0f, 0.0f, 0.0f };
        constexpr Vector4 One  = { 1.0f, 1.0f, 1.0f, 1.0f };

        constexpr Vector4 UnitX = { 1.0f, 0.0f, 0.0f, 0.0f };
        constexpr Vector4 UnitY = { 0.0f, 1.0f, 0.0f, 0.0f };
        constexpr Vector4 UnitZ = { 0.0f, 0.0f, 1.0f, 0.0f };
        constexpr Vector4 UnitW = { 0.0f, 0.0f, 0.0f, 1.0f };
#endif
    }

    namespace QuatConstants {
#if !FOW_CONSTEXPR_ENABLED
        const Quat Identity;
#else
        constexpr Quat Identity = { 1.0f, 0.0f, 0.0f, 0.0f };
#endif
    }

    namespace Matrix4Constants {
#if !FOW_CONSTEXPR_ENABLED
        const Matrix4 Identity;
#else
        constexpr Matrix4 Identity = { 1.0f };
#endif
    }

    namespace ColorConstants {
#if !FOW_CONSTEXPR_ENABLED
        const Color White;
        const Color Black;
        const Color Red;
        const Color Green;
        const Color Blue;
        const Color Yellow;
        const Color Cyan;
        const Color Magenta;
        const Color Transparent;

        const Color Gray;
        const Color Grey;
        const Color DarkGray;
        const Color DarkGrey;
        const Color LightGray;
        const Color LightGrey;

        const Color NormalMap;
#else
        constexpr Color White       = { 1.0f, 1.0f, 1.0f, 1.0f };
        constexpr Color Black       = { 0.0f, 0.0f, 0.0f, 1.0f };
        constexpr Color Red         = { 1.0f, 0.0f, 0.0f, 1.0f };
        constexpr Color Green       = { 0.0f, 1.0f, 0.0f, 1.0f };
        constexpr Color Blue        = { 0.0f, 0.0f, 1.0f, 1.0f };
        constexpr Color Yellow      = { 1.0f, 1.0f, 0.0f, 1.0f };
        constexpr Color Cyan        = { 0.0f, 1.0f, 1.0f, 1.0f };
        constexpr Color Magenta     = { 1.0f, 0.0f, 1.0f, 1.0f };
        constexpr Color Transparent = { 0.0f, 0.0f, 0.0f, 0.0f };

        constexpr Color Gray        = { 0.5f, 0.5f, 0.5f, 1.0f };
        constexpr Color Grey        = { 0.5f, 0.5f, 0.5f, 1.0f };
        constexpr Color DarkGray    = { 0.25f, 0.25f, 0.25f, 1.0f };
        constexpr Color DarkGrey    = { 0.25f, 0.25f, 0.25f, 1.0f };
        constexpr Color LightGray   = { 0.75f, 0.75f, 0.75f, 1.0f };
        constexpr Color LightGrey   = { 0.75f, 0.75f, 0.75f, 1.0f };

        constexpr Color NormalMap   = { 0.5f, 0.5f, 1.0f, 1.0f };
#endif
    }

    FOW_CONSTEXPR bool Rectangle::intersects(const Rectangle& rect) const {
        return  is_point_inside(rect.position()) ||
                is_point_inside(rect.position() + Vector2Constants::UnitX * rect.width) ||
                is_point_inside(rect.position() + Vector2Constants::UnitY * rect.height) ||
                is_point_inside(rect.position() + rect.size());
    }

    FOW_CONSTEXPR bool IntRectangle::intersects(const IntRectangle& rect) const {
        return  is_point_inside(rect.position()) ||
                is_point_inside(rect.position() + Vector2iConstants::UnitX * rect.width) ||
                is_point_inside(rect.position() + Vector2iConstants::UnitY * rect.height) ||
                is_point_inside(rect.position() + rect.size());
    }
}

#endif