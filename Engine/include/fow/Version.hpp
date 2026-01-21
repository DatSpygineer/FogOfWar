#ifndef FOW_VERSION_HPP
#define FOW_VERSION_HPP

#include <format>

#include "fow/Shared.hpp"
#include "fow/String.hpp"

namespace fow {
    struct FOW_SHARED_API Version {
        uint8_t major, minor, patch;
        uint32_t build;

        Version(const uint8_t major, const uint8_t minor, const uint8_t patch) : major(major), minor(minor), patch(patch), build() {
            const auto t = time(nullptr);
            const auto tm = localtime(&t);
            build = (tm->tm_year + 1900) * 10000 + (tm->tm_mon + 1) * 100 + (tm->tm_mday);
        }
        Version(const uint8_t major, const uint8_t minor, const uint8_t patch, const uint32_t build) : major(major), minor(minor), patch(patch), build(build) { }

    private:
        constexpr uint64_t as_u64() const {
            return (static_cast<uint64_t>(major << 24 | minor << 16 | patch << 8) << 32) | static_cast<uint64_t>(build);
        }
    public:
        constexpr auto operator<=>(const Version& other) const {
            return as_u64() <=> other.as_u64();
        }

        String to_string() const;
    };
}

template<>
struct std::formatter<fow::Version> {
    constexpr auto parse(std::format_parse_context& ctx) {
        return ctx.end();
    }

    auto format(const fow::Version& version, std::format_context& ctx) {
        return std::format_to(ctx.out(), "v{}.{}.{} {}", version.major, version.minor, version.patch, version.build);
    }
};

#endif