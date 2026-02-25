#ifndef FOW_STRING_HPP
#define FOW_STRING_HPP

#include "fow/Shared/Api.hpp"
#include "fow/Shared/Aliases.hpp"
#include "fow/Shared/MathHelper.hpp"

#include <stdexcept>
#include <string>
#include <cstring>
#include <filesystem>
#include <format>
#include <vector>

#include <nlohmann/json.hpp>
#include <pugixml.hpp>

namespace fow {
    class String;

    enum class StringCompareType {
        CaseSensitive,
        CaseInsensitive
    };

    class StringIterator;
    class StringConstIterator;

    class FOW_SHARED_API String {
        char* m_pData;
        size_t m_uSize, m_uCapacity;
    public:
        using iterator = StringIterator;
        using const_iterator = StringConstIterator;

        String() : String(static_cast<size_t>(0)) { }
        explicit String(size_t capacity);
        String(char c, size_t len);
        String(const char* cstr);             // NOLINT: Intentional conversion
        String(const char* cstr, size_t len);
        String(const std::string& str);       // NOLINT: Intentional conversion
        String(const std::string_view& str);  // NOLINT: Intentional conversion
        String(const String& str);
        String(String&& str) noexcept;
        ~String();

        String& operator=(const char* cstr);
        String& operator=(const std::string& str);
        String& operator=(const String& str);
        String& operator=(String&& str) noexcept;

        [[nodiscard]] FOW_CONSTEXPR size_t size()      const { return m_uSize; }
        [[nodiscard]] FOW_CONSTEXPR size_t capacity()  const { return m_uCapacity; }
        [[nodiscard]] FOW_CONSTEXPR const char* data() const { return m_pData; }
        [[nodiscard]] FOW_CONSTEXPR char* data()             { return m_pData; }
        [[nodiscard]] FOW_CONSTEXPR const char* as_cstr() const { return m_pData; }
        [[nodiscard]] std::string as_std_str() const;
        [[nodiscard]] FOW_CONSTEXPR bool is_empty() const { return m_pData == nullptr || m_pData[0] == '\0'; }
        [[nodiscard]] FOW_CONSTEXPR bool is_nullptr() const { return m_pData == nullptr; }

        [[nodiscard]] bool equals(const String& str, StringCompareType compare_type = StringCompareType::CaseSensitive) const;
        [[nodiscard]] bool equals_any(const std::initializer_list<String>& strs, StringCompareType compare_type = StringCompareType::CaseSensitive) const;
        [[nodiscard]] bool equals_slice(size_t this_offset, const String& str, size_t offset, size_t len, StringCompareType compare_type = StringCompareType::CaseSensitive) const;
        [[nodiscard]] inline bool equals_slice(const String& str, const size_t len, const StringCompareType compare_type = StringCompareType::CaseSensitive) const {
            return equals_slice(0, str, 0, len, compare_type);
        }
        [[nodiscard]] inline bool equals_slice(const String& str, const size_t offset, const size_t len, const StringCompareType compare_type = StringCompareType::CaseSensitive) const {
            return equals_slice(0, str, offset, len, compare_type);
        }

        [[nodiscard]] String substr(size_t start) const;
        [[nodiscard]] String substr(size_t start, size_t size) const;

        String& resize(size_t capacity);
        String& shrink_to_fit();
        String& recalculate_size();

        String& append(char c);
        String& append(const String& str);
        String& pop();
        String& insert(size_t idx, char c);
        String& insert(size_t idx, const String& str);
        String& remove(size_t start, size_t length = 1);
        String& remove(iterator it);
        String& remove(iterator _begin, iterator _end);
        String& remove_prefix(char prefix);
        String& remove_prefix(const String& prefix);
        String& remove_to_end(size_t start);
        String& replace(size_t idx, size_t original_length, const String& replacement);
        String& replace(char original, char replacement, size_t offset = 0);
        String& replace(const String& original, const String& replacement, size_t offset = 0);
        String& replace_all(char original, char replacement, size_t offset = 0);
        String& replace_all(const String& original, const String& replacement, size_t offset = 0);
        inline String& clear() {
            remove_to_end(0);
            m_uSize = 0;
            m_pData[0] = '\0';
            return *this;
        }

        [[nodiscard]] size_t count(char c, size_t offset = 0) const;
        [[nodiscard]] ssize_t find(char c, size_t offset = 0) const;
        [[nodiscard]] ssize_t find(const String& str, size_t offset = 0) const;
        [[nodiscard]] ssize_t find_last(char c, size_t offset = 0) const;
        [[nodiscard]] ssize_t find_last(const String& str, size_t offset = 0) const;
        [[nodiscard]] ssize_t find_any_first_of(const String& chars, size_t offset = 0) const;
        [[nodiscard]] ssize_t find_any_last_of(const String& chars, size_t offset = 0) const;
        [[nodiscard]] ssize_t find_any_first_not_of(const String& chars, size_t offset = 0) const;
        [[nodiscard]] ssize_t find_any_last_not_of(const String& chars, size_t offset = 0) const;
        [[nodiscard]] bool starts_with(char c, StringCompareType compare_type = StringCompareType::CaseSensitive) const;
        [[nodiscard]] bool starts_with(const String& str, StringCompareType compare_type = StringCompareType::CaseSensitive) const;
        [[nodiscard]] bool ends_with(char c, StringCompareType compare_type = StringCompareType::CaseSensitive) const;
        [[nodiscard]] bool ends_with(const String& str, StringCompareType compare_type = StringCompareType::CaseSensitive) const;

        String& trim_begin(const String& chars = " \t\n\r");
        String& trim_end(const String& chars = " \t\n\r");
        String& trim(const String& chars = " \t\n\r");
        String& to_lowercase();
        String& to_uppercase();

        [[nodiscard]] String clone_begin_trimmed(const String& chars = " \t\n\r") const;
        [[nodiscard]] String clone_end_trimmed(const String& chars = " \t\n\r") const;
        [[nodiscard]] String clone_trimmed(const String& chars = " \t\n\r") const;
        [[nodiscard]] String clone_lowercase() const;
        [[nodiscard]] String clone_uppercase() const;
        [[nodiscard]] Vector<String> split() const;
        [[nodiscard]] Vector<String> split(char c) const;

        static String Join(const Vector<String>& tokens, char delim);
        static String Join(const Vector<String>& tokens, const String& delim);

        [[nodiscard]] inline bool operator==(const String& other) const { return equals(other);  }
        [[nodiscard]] inline bool operator==(const std::nullptr_t&) const { return m_pData == nullptr;  }
        [[nodiscard]] inline bool operator!=(const String& other) const { return !equals(other); }
        [[nodiscard]] inline bool operator!=(const std::nullptr_t&) const { return m_pData != nullptr;  }
        [[nodiscard]] FOW_CONSTEXPR char& operator[] (const size_t idx) {
            if (idx > m_uSize) throw std::out_of_range("Index out of range!");
            return m_pData[idx];
        }
        [[nodiscard]] FOW_CONSTEXPR const char& operator[] (const size_t idx) const {
            if (idx > m_uSize) throw std::out_of_range("Index out of range!");
            return m_pData[idx];
        }
#if __cplusplus >= 202302L
        [[nodiscard]] inline String operator[] (const size_t start, const size_t length) const {
            return substr(start, length);
        }
#endif
        [[nodiscard]] inline String operator+ (const char c) const {
            String result = *this;
            result.append(c);
            return std::move(result);
        }
        [[nodiscard]] inline String operator+ (const String& str) const {
            String result = *this;
            result.append(str);
            return std::move(result);
        }
        [[nodiscard]] bool operator<(const String& other) const;
        [[nodiscard]] bool operator>(const String& other) const;
        inline String& operator+= (const char c) {
            return append(c);
        }
        inline String& operator+= (const String& str) {
            return append(str);
        }

        inline friend std::ostream& operator<< (std::ostream& os, const String& str) {
            os << str.as_std_str();
            return os;
        }
        inline friend std::istream& operator>> (std::istream& is, String& str) {
            std::string std_str;
            is >> std_str;
            str = std_str;
            return is;
        }

        [[nodiscard]] iterator begin();
        [[nodiscard]] const_iterator begin() const;
        [[nodiscard]] iterator end();
        [[nodiscard]] const_iterator end() const;

        friend class StringIterator;
        friend class StringConstIterator;

        static const size_t NotFound;
    };

    inline String operator""_s(const char* cstr, const size_t len) {
        return String { cstr, len };
    }

    class FOW_SHARED_API StringIterator {
        String& m_object;
        size_t m_uIndex;
    public:
        using iterator_category = std::random_access_iterator_tag;
        using value_type        = char;
        using difference_type   = std::ptrdiff_t;
        using pointer           = char*;
        using reference         = char&;

        StringIterator(const StringIterator&) = default;
        StringIterator(StringIterator&&) noexcept = default;
        StringIterator(String& obj, const size_t offset) : m_object(obj), m_uIndex(offset) { }

        [[nodiscard]] FOW_CONSTEXPR size_t index() const { return m_uIndex; }

        inline StringIterator& operator++() {
            ++m_uIndex;
            return *this;
        }
        inline StringIterator operator++(int) {
            const StringIterator tmp(*this);
            ++m_uIndex;
            return tmp;
        }
        inline StringIterator& operator--() {
            --m_uIndex;
            return *this;
        }
        inline StringIterator operator--(int) {
            const StringIterator tmp(*this);
            --m_uIndex;
            return tmp;
        }
        FOW_CONSTEXPR bool operator==(const StringIterator& other) const {
            if (m_uIndex >= m_object.m_uSize) {
                return other.m_uIndex >= m_object.m_uSize;
            }
            return m_uIndex == other.m_uIndex;
        }
        FOW_CONSTEXPR bool operator!=(const StringIterator& other) const {
            return !operator==(other);
        }
        inline char& operator*() const {
            if (m_uIndex >= m_object.m_uSize) {
                throw std::out_of_range("Index out of range!");
            }
            return m_object.m_pData[m_uIndex];
        }

        inline StringIterator operator+(const size_t offset) const {
            return StringIterator { m_object, m_uIndex + offset };
        }
        inline StringIterator& operator+=(const size_t offset) {
            m_uIndex += offset;
            return *this;
        }
        inline StringIterator operator-(const size_t offset) const {
            return StringIterator { m_object, m_uIndex - offset };
        }
        inline difference_type operator-(const StringIterator& it) const {
            return static_cast<difference_type>(m_uIndex) - static_cast<difference_type>(it.m_uIndex);
        }
        difference_type operator-(const StringConstIterator& it) const;
        inline StringIterator& operator-=(const size_t offset) {
            m_uIndex -= offset;
            return *this;
        }

        friend class StringConstIterator;
    };
    class FOW_SHARED_API StringConstIterator {
        const String& m_object;
        size_t m_uIndex;
    public:
        using iterator_category = std::random_access_iterator_tag;
        using value_type        = char;
        using difference_type   = std::ptrdiff_t;
        using pointer           = const char*;
        using reference         = const char&;

        StringConstIterator(const StringConstIterator&) = default;
        StringConstIterator(StringConstIterator&&) noexcept = default;
        StringConstIterator(const String& obj, const size_t offset) : m_object(obj), m_uIndex(offset) { }
        StringConstIterator(const StringIterator& it) : m_object(it.m_object), m_uIndex(it.m_uIndex) { }

        [[nodiscard]] FOW_CONSTEXPR size_t index() const { return m_uIndex; }

        inline StringConstIterator& operator++() {
            ++m_uIndex;
            return *this;
        }
        inline StringConstIterator operator++(int) {
            const StringConstIterator tmp(*this);
            ++m_uIndex;
            return tmp;
        }
        inline StringConstIterator& operator--() {
            --m_uIndex;
            return *this;
        }
        inline StringConstIterator operator--(int) {
            const StringConstIterator tmp(*this);
            --m_uIndex;
            return tmp;
        }
        FOW_CONSTEXPR bool operator==(const StringConstIterator& other) const {
            if (m_uIndex >= m_object.m_uSize) {
                return other.m_uIndex >= m_object.m_uSize;
            }
            return m_uIndex == other.m_uIndex;
        }
        FOW_CONSTEXPR bool operator!=(const StringConstIterator& other) const {
            return !operator==(other);
        }
        inline const char& operator*() const {
            if (m_uIndex >= m_object.m_uSize) {
                throw std::out_of_range("Index out of range!");
            }
            return m_object.m_pData[m_uIndex];
        }

        inline StringConstIterator operator+(const size_t offset) const {
            return StringConstIterator { m_object, m_uIndex + offset };
        }
        inline StringConstIterator& operator+=(const size_t offset) {
            m_uIndex += offset;
            return *this;
        }
        inline StringConstIterator operator-(const size_t offset) const {
            return StringConstIterator { m_object, m_uIndex - offset };
        }
        inline difference_type operator-(const StringIterator& it) const {
            return static_cast<difference_type>(m_uIndex) - static_cast<difference_type>(it.m_uIndex);
        }
        inline difference_type operator-(const StringConstIterator& it) const {
            return static_cast<difference_type>(m_uIndex) - static_cast<difference_type>(it.m_uIndex);
        }
        inline StringConstIterator& operator-=(const size_t offset) {
            m_uIndex -= offset;
            return *this;
        }
        friend class StringIterator;
    };

    FOW_SHARED_API bool PathMatches(const std::filesystem::path& path, const std::string& pattern);

    class FOW_SHARED_API Path {
        String m_sPath;
    public:
        Path() = default;
        Path(const Path& path) = default;
        Path(Path&& path) noexcept = default;
        Path(const char* cstr) : Path(std::move(String(cstr))) { }                               // NOLINT: Intended conversion
        explicit Path(const std::string& str) : Path(std::move(String(str))) { }                          // NOLINT: Intended conversion
        Path(const std::filesystem::path& path) : Path(std::move(String(path.string()))) { } // NOLINT: Intended conversion
        Path(const String& path);                                                                // NOLINT: Intended conversion
        Path(String&& path) noexcept;                                                            // NOLINT: Intended conversion

        [[nodiscard]] String filename() const;
        [[nodiscard]] String filename_without_extension() const;
        [[nodiscard]] String extension() const;
        [[nodiscard]] Path parent() const;
        [[nodiscard]] inline Path root() const { return parent(); }
        [[nodiscard]] std::tuple<Path, String> split() const;
        [[nodiscard]] std::tuple<Path, String> split_ext() const;
        [[nodiscard]] Path as_absolute() const;
        [[nodiscard]] Path as_absolute(const Path& root) const;
        [[nodiscard]] Path as_relative() const;
        [[nodiscard]] Path as_relative(const Path& root) const;
        [[nodiscard]] FOW_CONSTEXPR String& as_string() { return m_sPath; }
        [[nodiscard]] FOW_CONSTEXPR const String& as_string() const { return m_sPath; }
        [[nodiscard]] FOW_CONSTEXPR const char* as_cstr() const { return m_sPath.as_cstr(); }
#if __cplusplus > 202002L
        [[nodiscard]] FOW_CONSTEXPR std::string as_std_str() const { return m_sPath.as_std_str(); }
        [[nodiscard]] FOW_CONSTEXPR std::filesystem::path as_std_path() const { return m_sPath.as_std_str(); }
#else
        [[nodiscard]] inline std::string as_std_str() const { return m_sPath.as_std_str(); }
        [[nodiscard]] inline std::filesystem::path as_std_path() const { return m_sPath.as_std_str(); }
#endif
        [[nodiscard]] FOW_CONSTEXPR bool is_empty() const { return m_sPath.is_empty(); }
        [[nodiscard]] Vector<Path> list_dir() const;
        [[nodiscard]] Vector<Path> list_dir(const String& filter) const;
        [[nodiscard]] Vector<Path> list_dir(const std::initializer_list<String>& filters) const;

        Path& replace_filename(const String& name);
        Path& replace_filename_and_extension(const String& name);
        inline Path& replace_filename_and_extension(const String& name, const String& extension) {
            return replace_filename_and_extension(std::format("{}.{}", name.as_cstr(), extension.as_cstr()));
        }
        Path& replace_extension(const String& ext);
        Path& replace_root(const Path& new_root);
        Path& replace_root(const Path& old_root, const Path& new_root);
        Path& to_absolute();
        Path& to_relative();
        Path& to_relative(const Path& root);
        Path& go_back();

        static Path CurrentDir();
        static void CurrentDir(const Path& path);
        static Path Join(const std::initializer_list<Path>& paths);
        static bool CreateDirectory(const Path& path, bool recursive = false);
        static bool Remove(const Path& path, bool recursive = false);
        static bool Copy(const Path& src, const Path& dest);
        static bool Move(const Path& src, const Path& dest);
        inline static bool Rename(const Path& src, const Path& dest) {
            return Move(src, dest);
        }

        [[nodiscard]] bool exists() const;
        [[nodiscard]] bool is_file() const;
        [[nodiscard]] bool is_directory() const;
        [[nodiscard]] bool is_absolute() const;
        [[nodiscard]] bool matches(const String& pattern) const;

        bool remove() const;
        bool remove_all() const;

        [[nodiscard]] Path operator+ (const String& other) const;
        [[nodiscard]] Path operator/ (const char* other) const;
        [[nodiscard]] Path operator/ (const String& other) const;
        [[nodiscard]] Path operator/ (const Path& other) const;

        Path& operator+= (const String& other);
        Path& operator/= (const char* other);
        Path& operator/= (const String& other);
        Path& operator/= (const Path& other);

        inline bool operator== (const Path& path) const {
            return m_sPath == path.m_sPath;
        }
        inline bool operator!= (const Path& path) const {
            return m_sPath != path.m_sPath;
        }

        Path& operator= (const Path& other) = default;
        Path& operator= (Path&& other) noexcept = default;

        inline friend std::ostream& operator<<(std::ostream& os, const Path& path) {
            os << path.m_sPath;
            return os;
        }
        inline friend std::istream& operator>>(std::istream& is, Path& path) {
            is >> path.m_sPath;
            return is;
        }
    };

    inline Path operator""_p(const char* cstr, const size_t len) {
        return Path { std::move(String { cstr, len }) };
    }
}

template<>
struct std::formatter<fow::String> : std::formatter<std::string> {
    inline auto format(const fow::String& str, std::format_context& ctx) const {
        return std::formatter<std::string>::format(str.as_cstr(), ctx);
    }
};
template<>
struct std::formatter<fow::Path> : std::formatter<std::string> {
    inline auto format(const fow::Path& path, std::format_context& ctx) const {
        return std::formatter<std::string>::format(path.as_cstr(), ctx);
    }
};

template<>
struct std::hash<fow::String> {
    auto operator() (const fow::String& str) const noexcept {
        return std::hash<std::string>()(str.as_std_str());
    }
};
template<>
struct std::hash<fow::Path> {
    auto operator() (const fow::Path& path) const noexcept {
        return std::hash<std::string>()(path.as_std_str());
    }
};

template<>
struct std::formatter<glm::vec2> {
    FOW_CONSTEXPR auto parse(std::format_parse_context& ctx) {
        return ctx.end();
    }
    inline auto format(const glm::vec2& value, std::format_context& ctx) const {
        return std::format_to(ctx.out(), "{}, {}", value.x, value.y);
    }
};
template<>
struct std::formatter<glm::vec3> {
    FOW_CONSTEXPR auto parse(std::format_parse_context& ctx) {
        return ctx.end();
    }
    inline auto format(const glm::vec3& value, std::format_context& ctx) const {
        return std::format_to(ctx.out(), "{}, {}, {}", value.x, value.y, value.z);
    }
};
template<>
struct std::formatter<glm::vec4> {
    FOW_CONSTEXPR auto parse(std::format_parse_context& ctx) {
        return ctx.end();
    }
    inline auto format(const glm::vec4& value, std::format_context& ctx) const {
        return std::format_to(ctx.out(), "{}, {}, {}, {}", value.x, value.y, value.z, value.w);
    }
};
template<>
struct std::formatter<glm::quat> : std::formatter<glm::vec4> {
    FOW_CONSTEXPR auto parse(std::format_parse_context& ctx) {
        return ctx.end();
    }
    inline auto format(const glm::quat& value, std::format_context& ctx) const {
        return std::formatter<glm::vec4>::format(glm::vec4(value.x, value.y, value.z, value.w), ctx);
    }
};

template<>
struct std::formatter<fow::Color> : std::formatter<std::string> {
    fow::ColorFormat m_format = fow::ColorFormat::RgbaFloat;
    bool m_bHexLowerCase = false, m_bHexPrefix = true;

    FOW_CONSTEXPR auto parse(std::format_parse_context& ctx) {
        auto it = ctx.begin();
        if (it == ctx.end()) {
            return it;
        }

        if (*it == '#' || *it == 'x' || *it == 'X') {
            m_format = fow::ColorFormat::Hex;
            m_bHexPrefix = *it == '#';
            m_bHexLowerCase = *it == 'x';
        } else if (*it == 'b' || *it == 'B') {
            m_format = fow::ColorFormat::RgbaBytes;
        }
        ++it;
        return std::formatter<std::string>::parse(ctx);
    }
    inline auto format(const fow::Color& value, std::format_context& ctx) const {
        std::string result;
        switch (m_format) {
            case fow::ColorFormat::RgbaFloat: {
                result = std::format("{}, {}, {}, {}", value.r, value.g, value.b, value.a);
            } break;
            case fow::ColorFormat::RgbaBytes: {
                const auto [ r, g, b, a ] = value.to_bytes();
                result = std::format("{}, {}, {}, {}", r, g, b, a);
            } break;
            case fow::ColorFormat::Hex: {
                result = m_bHexPrefix ? "#" : "";
                if (m_bHexLowerCase) {
                    result += std::format("{:08x}", value.to_int());
                } else {
                    result += std::format("{:08X}", value.to_int());
                }
            } break;
        }
        return std::formatter<std::string>::format(result, ctx);
    }
};

#if __cplusplus < 202302L
#include <iostream>

namespace std {
    template<typename ...Args>
    void print(const std::format_string<Args...>& format, Args&&... args) {
        std::cout << std::format<Args...>(format, std::forward<Args>(args)...);
    }
    template<typename ...Args>
    void print(std::ostream& os, const std::format_string<Args...>& format, Args&&... args) {
        os << std::format<Args...>(format, std::forward<Args>(args)...);
    }
    template<typename ...Args>
    void println(const std::format_string<Args...>& format, Args&&... args) {
        std::cout << std::format<Args...>(format, std::forward<Args>(args)...) << std::endl;
    }
    template<typename ...Args>
    void println(std::ostream& os, const std::format_string<Args...>& format, Args&&... args) {
        os << std::format<Args...>(format, std::forward<Args>(args)...) << std::endl;
    }
}
#endif

#endif