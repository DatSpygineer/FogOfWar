#ifndef FOW_RESULT_HPP
#define FOW_RESULT_HPP

#include <expected>
#include <source_location>
#include <variant>

#include "fow/Shared/Api.hpp"
#include "fow/Shared/String.hpp"
#include "rfl/Result.hpp"

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

    using NoneType = std::monostate;

#if __cplusplus >= 202302L
    template<typename T = NoneType>
    using Result       = std::expected<T, Error>;
    using FailedResult = std::unexpected<Error>;

    constexpr Result<> Success() { return Result<NoneType>(NoneType { }); }
    template<typename T>
    constexpr Result<T> Success(const T& value) { return Result<T>(value); }
    template<typename T>
    constexpr Result<T> Success(T&& value) noexcept { return Result<T>(std::forward<T>(value)); }
    constexpr FailedResult Failure(const String& message, const std::source_location& location = std::source_location::current()) { return std::unexpected(Error(message, location)); }
    constexpr FailedResult Failure(const Error& error) { return std::unexpected(error); }
    constexpr FailedResult Failure(Error&& error) noexcept { return std::unexpected(std::forward<Error>(error)); }
#else
    #if __cplusplus < 202002L
        #error "Versions below C++ 20 are not supported!"
    #endif

    class ResultHasNoValueException : public std::exception {
    public:
        ResultHasNoValueException() noexcept = default;
        [[nodiscard]] constexpr const char* what() const noexcept override { return "Result type has no value!"; }
    };
    class ResultHasNoErrorException : public std::exception {
    public:
        ResultHasNoErrorException() noexcept = default;
        [[nodiscard]] constexpr const char* what() const noexcept override { return "Result type has no Error!"; }
    };

    using FailedResult = Error;

    template<typename T = NoneType>
    class Result {
        std::variant<T, Error> m_value;
    public:
        constexpr Result() : m_value(Error("Not implemented")) { }
        constexpr Result(const T& value) : m_value(value) { }
        constexpr Result(T&& value) noexcept : m_value(std::move(value)) { }
        constexpr Result(const Error& error) : m_value(error) { }
        constexpr Result(Error&& error) noexcept : m_value(std::move(error)) { }

        [[nodiscard]] constexpr bool has_value() const { return m_value.index() == 0; }
        [[nodiscard]] constexpr const T& value() const {
            if (m_value.index() != 0) {
                throw ResultHasNoValueException();
            }
            return std::get<0>(m_value);
        }
        [[nodiscard]] constexpr const T& value_or(const T& _default) const {
            return m_value.index() == 0 ? std::get<0>(m_value) : _default;
        }
        [[nodiscard]] constexpr const Error& error() const {
            if (m_value.index() == 0) {
                throw ResultHasNoErrorException();
            }
            return std::get<1>(m_value);
        }
        constexpr T* operator->() {
            if (m_value.index() != 0) {
                throw ResultHasNoValueException();
            }
            return &std::get<0>(m_value);
        }
        constexpr const T* operator->() const {
            if (m_value.index() != 0) {
                throw ResultHasNoValueException();
            }
            return &std::get<0>(m_value);
        }
        constexpr T& operator*() {
            if (m_value.index() != 0) {
                throw ResultHasNoValueException();
            }
            return std::get<0>(m_value);
        }
        constexpr const T& operator*() const {
            if (m_value.index() != 0) {
                throw ResultHasNoValueException();
            }
            return std::get<0>(m_value);
        }
    };

    constexpr Result<> Success() { return Result<NoneType>(NoneType { }); }
    template<typename T>
    constexpr Result<T> Success(const T& value) { return Result<T>(value); }
    template<typename T>
    constexpr Result<T> Success(T&& value) noexcept { return Result<T>(std::forward<T>(value)); }
    inline FailedResult Failure(const String& message, const std::source_location& location = std::source_location::current()) { return FailedResult(message, location); }
    inline FailedResult Failure(const Error& error) { return { error }; }
    inline FailedResult Failure(Error&& error) noexcept { return { std::move(error) }; }
#endif
}

#endif