#ifndef FOW_DEBUG_HPP
#define FOW_DEBUG_HPP

#include <fow/Shared.hpp>
#include <fow/String.hpp>
#include <fow/Result.hpp>

#include <format>
#include <functional>
#include <print>
#include <source_location>

namespace fow {
    enum class LogLevel {
        Debug,
        Info,
        Warning,
        Error,
        Fatal
    };
    namespace Debug {
        FOW_SHARED_API void Initialize(const Path& log_base_dir);
        FOW_SHARED_API void Terminate();

        using LogMessageCallback = std::function<void(LogLevel, const Time::TimePoint&, const String&, const std::source_location&)>;

        FOW_SHARED_API void SetMessageSentCallback(const LogMessageCallback& callback);
        FOW_SHARED_API void SetMinimumLogLevel(LogLevel level);

        FOW_SHARED_API void Log(LogLevel level, const String& message, const std::source_location& location = std::source_location::current());
        inline void LogDebug(const String& message, const std::source_location& location = std::source_location::current()) {
            Log(LogLevel::Debug, message, location);
        }
        inline void LogInfo(const String& message, const std::source_location& location = std::source_location::current()) {
            Log(LogLevel::Info, message, location);
        }
        inline void LogWarning(const String& message, const std::source_location& location = std::source_location::current()) {
            Log(LogLevel::Warning, message, location);
        }
        inline void LogError(const String& message, const std::source_location& location = std::source_location::current()) {
            Log(LogLevel::Error, message, location);
        }
        inline void LogFatal(const String& message, const std::source_location& location = std::source_location::current()) {
            Log(LogLevel::Fatal, message, location);
        }
        bool Assert(bool condition, const String& fail_message, const std::source_location& location = std::source_location::current());
        template<typename T = std::monostate>
        inline bool Assert(const Result<T>& result) {
            if (!result.has_value()) {
                LogError(result.error().message, result.error().location);
                return false;
            }
            return true;
        }
        bool AssertFatal(bool condition, const String& fail_message, const std::source_location& location = std::source_location::current());
        template<typename T = std::monostate>
        inline bool AssertFatal(const Result<T>& result) {
            if (!result.has_value()) {
                LogFatal(result.error().message, result.error().location);
                return false;
            }
            return true;
        }
    }
}

template<>
struct std::formatter<fow::LogLevel> : std::formatter<std::string_view> {
    auto format(const fow::LogLevel level, std::format_context& ctx) const {
        std::string_view result;
        switch (level) {
            case fow::LogLevel::Debug:   result = "DEBUG";   break;
            case fow::LogLevel::Info:    result = "INFO";    break;
            case fow::LogLevel::Warning: result = "WARNING"; break;
            case fow::LogLevel::Error:   result = "ERROR";   break;
            case fow::LogLevel::Fatal:   result = "FATAL";   break;
        }
        return std::formatter<std::string_view>::format(result, ctx);
    }
};

#endif