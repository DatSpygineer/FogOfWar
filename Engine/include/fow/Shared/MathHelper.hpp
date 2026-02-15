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

    class FOW_SHARED_API Color : public glm::vec4 {
    public:
        FOW_CONSTEXPR Color() : glm::vec4(0.0f, 0.0f, 0.0f, 1.0f) { }
        FOW_CONSTEXPR Color(const float r, const float g, const float b, const float a = 1.0f) : glm::vec4(r, g, b, a) { }

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
        FOW_CONSTEXPR Rectangle(const glm::vec2& position, const glm::vec2& size) : x(position.x), y(position.y), width(size.x), height(size.y) { }
        FOW_CONSTEXPR Rectangle(const float x, const float y, const float width, const float height) : x(x), y(y), width(width), height(height) { }
        FOW_CONSTEXPR Rectangle(const float x, const float y, const glm::vec2& size) : x(x), y(y), width(size.x), height(size.y) { }
        FOW_CONSTEXPR Rectangle(const glm::vec2& position, const float width, const float height) : x(position.x), y(position.y), width(width), height(height) { }

        Rectangle& operator=(const Rectangle& other)     = default;
        Rectangle& operator=(Rectangle&& other) noexcept = default;
    };
}

#endif