#ifndef FOW_STRING_CONVERTION_HPP
#define FOW_STRING_CONVERTION_HPP

#include "fow/Shared/Api.hpp"
#include "fow/Shared/String.hpp"
#include "fow/Shared/Result.hpp"

#include <rfl.hpp>

template<typename T>
concept EnumType = std::is_enum_v<T>;

namespace fow {
    template<IntegerType T>
    FOW_SHARED_API Result<T> StringToInt(const String& str, int radix = 0);
    template<FloatType T>
    FOW_SHARED_API Result<T> StringToFloat(const String& str);
    FOW_SHARED_API Result<bool> StringToBool(const String& str);

    FOW_SHARED_API Result<glm::vec2> StringToVec2(const String& str);
    FOW_SHARED_API Result<glm::vec3> StringToVec3(const String& str);
    FOW_SHARED_API Result<glm::vec4> StringToVec4(const String& str);
    FOW_SHARED_API Result<glm::bvec2> StringToBVec2(const String& str);
    FOW_SHARED_API Result<glm::bvec3> StringToBVec3(const String& str);
    FOW_SHARED_API Result<glm::bvec4> StringToBVec4(const String& str);
    FOW_SHARED_API Result<glm::uvec2> StringToUVec2(const String& str);
    FOW_SHARED_API Result<glm::uvec3> StringToUVec3(const String& str);
    FOW_SHARED_API Result<glm::uvec4> StringToUVec4(const String& str);
    FOW_SHARED_API Result<glm::ivec2> StringToIVec2(const String& str);
    FOW_SHARED_API Result<glm::ivec3> StringToIVec3(const String& str);
    FOW_SHARED_API Result<glm::ivec4> StringToIVec4(const String& str);
    FOW_SHARED_API Result<glm::dvec2> StringToDVec2(const String& str);
    FOW_SHARED_API Result<glm::dvec3> StringToDVec3(const String& str);
    FOW_SHARED_API Result<glm::dvec4> StringToDVec4(const String& str);
    FOW_SHARED_API Result<glm::mat4> StringToMat4(const String& str);

    template<IntegerType T>
    Result<T> StringToInt(const String& str, int radix)  {
        size_t offset = 0;
        if (radix == 0) {
            if (str.starts_with("0x", StringCompareType::CaseInsensitive)) {
                radix = 16;
                offset = 2;
            } else if (str.starts_with("0b", StringCompareType::CaseInsensitive)) {
                radix = 2;
                offset = 2;
            } else if (str.starts_with("0o", StringCompareType::CaseInsensitive)) {
                radix = 8;
                offset = 2;
            } else if (str.starts_with("#")) {
                radix = 16;
                offset = 1;
            } else if (str.starts_with("0")) {
                radix = 8;
                offset = 1;
            } else {
                radix = 10;
            }
        }
        try {
            if (std::is_signed_v<T>) {
                return Success<T>(static_cast<T>(std::stoll((offset > 0 ? str.substr(offset) : str).as_std_str(), nullptr, radix)));
            } else {
                return Success<T>(static_cast<T>(std::stoull((offset > 0 ? str.substr(offset) : str).as_std_str(), nullptr, radix)));
            }
        } catch (const std::exception& ex) {
            return Failure(std::format("Failed to parse string \"{}\" to integer: {}", str, String { ex.what() }));
        }
    }
    template<FloatType T>
    Result<T> StringToFloat(const String& str) {
        try {
            return Success<T>(static_cast<T>(std::stod(str.as_std_str())));
        } catch (const std::exception& ex) {
            return Failure(std::format("Failed to parse string \"{}\" to float: {}", str, String { ex.what() }));
        }
    }
    template<EnumType T>
    Result<T> StringToEnum(const String& str) {
        if (const auto result = rfl::string_to_enum<T>(str); result.has_value()) {
            return result;
        } else {
            return Failure(result.error().what());
        }
    }
}

#endif