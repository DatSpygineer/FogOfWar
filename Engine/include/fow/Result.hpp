#ifndef FOW_RESULT_HPP
#define FOW_RESULT_HPP

#include <expected>
#include <source_location>
#include <variant>

#include "fow/Shared.hpp"
#include "fow/String.hpp"

namespace fow {
    struct FOW_SHARED_API Error {
        String message;
        std::source_location location;

        explicit Error(const String& message, const std::source_location& location = std::source_location::current()) :
            message(message), location(location) { }
        explicit Error(String&& message, const std::source_location& location = std::source_location::current()) :
            message(std::move(message)), location(location) { }
    };

    template<typename T>
    using Option = std::optional<T>;

    template<typename T>
    constexpr Option<T> Some(const T& value) { return std::make_optional(value); }
    template<typename T>
    constexpr Option<T> Some(T&& value) noexcept { return std::make_optional(std::forward<T>(value)); }
    constexpr auto None() { return std::nullopt; }

    template<typename T = std::monostate>
    using Result = std::expected<T, Error>;

    constexpr Result<> Success() { return Result(std::monostate { }); }
    template<typename T>
    constexpr Result<T> Success(const T& value) { return Result<T>(value); }
    template<typename T>
    constexpr Result<T> Success(T&& value) noexcept { return Result<T>(std::forward<T>(value)); }
    template<typename T = std::monostate>
    constexpr Result<T> Failure(const String& message, const std::source_location& location = std::source_location::current()) { return std::unexpected(Error(message, location)); }
    template<typename T = std::monostate>
    constexpr Result<T> Failure(const Error& error) { return std::unexpected(error); }
    template<typename T = std::monostate>
    constexpr Result<T> Failure(Error&& error) { return std::unexpected(std::forward<T>(error)); }
}

#endif