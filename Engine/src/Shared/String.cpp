#include "fow/Shared/String.hpp"
#include "fow/Shared/StringConvertion.hpp"
#include "fow/Shared/PointerHelper.hpp"

#include <sstream>

#ifdef _WIN32
    #define strcasecmp _stricmp
    #define strncasecmp _strnicmp
    #define FOW_PATH_SEPARATOR          "\\"
    #define FOW_PATH_SEPARATOR_CHAR     '\\'
    #define FOW_PATH_SEPARATOR_CHAR_ALT '/'
    #define FOW_PATH_SEPARATOR_ANY      "\\/"
#else
    #define FOW_PATH_SEPARATOR      "/"
    #define FOW_PATH_SEPARATOR_CHAR '/'
    #define FOW_PATH_SEPARATOR_CHAR_ALT '/'
    #define FOW_PATH_SEPARATOR_ANY  "/"
#endif

#ifdef __GNUC__
    #include <unistd.h>
    #include <fnmatch.h>
#else
#endif

namespace fow {
    String::String(const size_t capacity) : String('\0', capacity) { }
    String::String(const char c, const size_t len) : m_pData(new char[len + 1]), m_uSize(c != '\0' ? len : 0), m_uCapacity(len) {
        memset(m_pData, c, m_uCapacity + 1);
        m_pData[len] = '\0';
    }
    String::String(const char* cstr) : String(cstr, cstr == nullptr ? 0 : strlen(cstr)) { }
    String::String(const char* cstr, const size_t len) : m_pData(new char[(cstr != nullptr ? len : 0) + 1]), m_uSize(cstr == nullptr ? 0 : len), m_uCapacity(cstr == nullptr ? 0 : len) {
        strncpy(m_pData, cstr, len);
        m_pData[len] = '\0';
    }
    String::String(const std::string& str) : m_pData(new char[str.capacity() + 1]), m_uSize(str.size()), m_uCapacity(str.capacity()) {
        strncpy(m_pData, str.c_str(), str.size());
        m_pData[str.size()] = '\0';
    }
    String::String(const std::string_view& str) : m_pData(new char[str.size() + 1]), m_uSize(str.size()), m_uCapacity(str.size()) {
        strncpy(m_pData, str.data(), str.size());
        m_pData[str.size()] = '\0';
    }
    String::String(const String& str) : m_pData(new char[str.m_uCapacity + 1]), m_uSize(str.m_uSize), m_uCapacity(str.m_uCapacity) {
        memset(m_pData, 0, m_uCapacity + 1);
        strncpy(m_pData, str.m_pData, str.m_uSize);
    }
    String::String(String&& str) noexcept : m_pData(str.m_pData), m_uSize(str.m_uSize), m_uCapacity(str.m_uCapacity) {
        str.m_pData = nullptr;
        str.m_uSize = 0;
        str.m_uCapacity = 0;
    }
    String::~String() {
        delete[] m_pData;
    }

    String& String::operator=(const char* cstr) {
        delete[] m_pData;
        m_uSize = m_uCapacity = strlen(cstr);
        m_pData = new char[m_uCapacity + 1];
        strcpy(m_pData, cstr);
        m_pData[m_uSize] = '\0';
        return *this;
    }
    String& String::operator=(const std::string& str) {
        delete[] m_pData;
        m_uSize = str.size();
        m_uCapacity = str.capacity();
        m_pData = new char[m_uCapacity + 1];
        strcpy(m_pData, str.c_str());
        m_pData[m_uSize] = '\0';
        return *this;
    }
    String& String::operator=(const String& str) {
        if (this == &str || m_pData == str.m_pData) {
            return *this;
        }

        delete[] m_pData;
        m_uSize = str.size();
        m_uCapacity = str.capacity();
        m_pData = new char[m_uCapacity + 1];
        strcpy(m_pData, str.data());
        m_pData[m_uSize] = '\0';
        return *this;
    }
    String& String::operator=(String&& str) noexcept {
        if (this == &str || m_pData == str.m_pData) {
            return *this;
        }

        delete[] m_pData;
        m_pData = str.m_pData;
        m_uSize = str.m_uSize;
        m_uCapacity = str.m_uCapacity;

        str.m_pData = nullptr;
        str.m_uSize = 0;
        str.m_uCapacity = 0;
        return *this;
    }

    std::string String::as_std_str() const {
        return std::string { m_pData };
    }

    bool String::equals(const String& str, const StringCompareType compare_type) const {
        return (compare_type == StringCompareType::CaseSensitive ? strcmp(m_pData, str.m_pData) : strcasecmp(m_pData, str.m_pData)) == 0;
    }

    bool String::equals_any(const std::initializer_list<String>& strs, StringCompareType compare_type) const {
        for (const auto& str : strs) {
            if (equals(str, compare_type)) {
                return true;
            }
        }
        return false;
    }

    bool String::equals_slice(const size_t this_offset, const String& str, const size_t offset, const size_t len, const StringCompareType compare_type) const {
        const char* lhs = m_pData + this_offset;
        const char* rhs = str.m_pData + offset;
        return (compare_type == StringCompareType::CaseSensitive ? strncmp(lhs, rhs, len) : strncasecmp(lhs, rhs, len)) == 0;
    }

    String String::substr(const size_t start) const {
        if (start >= m_uSize) {
            return String { };
        }
        return String { m_pData + start };
    }
    String String::substr(const size_t start, const size_t size) const {
        if (start >= m_uSize) {
            return String { };
        }
        if (size >= m_uSize - start) {
            return String { m_pData + start };
        }
        return String { m_pData + start, size };
    }

    String& String::resize(const size_t capacity) {
        if (m_uCapacity < capacity) {
            if (m_pData != nullptr) {
                m_pData = ResizePtr(m_pData, m_uCapacity, capacity + 1);
                m_pData[m_uSize] = '\0';
            } else {
                m_pData = new char[capacity + 1];
                memset(m_pData, 0, capacity + 1);
            }
            m_uCapacity = capacity;
        }
        return *this;
    }

    String& String::shrink_to_fit() {
        if (m_pData != nullptr) {
            m_pData = ResizePtr(m_pData, m_uSize, m_uSize + 1);
            m_pData[m_uSize] = '\0';
            m_uCapacity = m_uSize;
        }
        return *this;
    }

    String& String::recalculate_size() {
        m_uSize = strlen(m_pData);
        return *this;
    }

    String& String::append(const char c) {
        if (m_uSize + 1 > m_uCapacity) {
            resize(m_uSize + 1);
        }
        m_pData[m_uSize++] = c;
        return *this;
    }
    String& String::append(const String& str) {
        if (m_uSize + str.m_uSize > m_uCapacity) {
            resize(m_uSize + str.m_uSize);
        }
        memcpy(m_pData + m_uSize, str.m_pData, str.m_uSize);
        m_uSize += str.m_uSize;
        m_pData[m_uSize] = '\0';
        return *this;
    }

    String& String::pop() {
        m_pData[--m_uSize] = '\0';
        return *this;
    }

    String& String::insert(const size_t idx, const char c) {
        if (idx >= m_uSize) {
            return append(c);
        }

        if (m_uCapacity < m_uSize + 1) {
            resize(m_uSize + 1);
        }

        memmove(m_pData + idx + 1, m_pData + idx, m_uSize - idx);
        m_pData[idx] = c;
        m_pData[++m_uSize] = '\0';
        return *this;
    }

    String& String::insert(const size_t idx, const String& str) {
        if (idx >= m_uSize) {
            return append(str);
        }

        if (m_uSize + 1 >= m_uCapacity) {
            resize(m_uSize + str.m_uSize);
        }

        memmove(m_pData + idx + str.m_uSize, m_pData + idx, m_uSize - idx);
        memcpy(m_pData + idx, str.m_pData, str.m_uSize);
        m_uSize += str.m_uSize;
        return *this;
    }

    String& String::remove(const size_t start, const size_t length) {
        if (start == m_uSize - 1) {
            return pop();
        }
        if (start >= m_uSize) {
            throw std::out_of_range("Index out of range!");
        }
        if (start + length > m_uSize) {
            throw std::out_of_range("Length is out of range!");
        }
        memmove(m_pData + start, m_pData + start + length, m_uSize - start - length);
        m_uSize -= length;
        m_pData[m_uSize] = '\0';
        return *this;
    }
    String& String::remove(const iterator it) {
        return remove(std::distance(begin(), it));
    }
    String& String::remove(const iterator _begin, const iterator _end) {
        const size_t begin_idx = std::distance(begin(), _begin);
        const size_t end_idx = std::distance(begin(), _end);
        if (_end == end()) {
            return remove_to_end(std::distance(begin(), _begin));
        }
        return remove(begin_idx, end_idx - begin_idx);
    }

    String& String::remove_prefix(const char prefix) {
        if (starts_with(prefix)) {
            remove(begin());
        }
        return *this;
    }
    String& String::remove_prefix(const String& prefix) {
        if (starts_with(prefix)) {
            remove(begin(), begin() + prefix.size());
        }
        return *this;
    }

    String& String::remove_to_end(const size_t start) {
        memset(m_pData + start, 0, m_uSize - start);
        m_uSize -= m_uSize - start;
        return *this;
    }

    String& String::replace(const size_t idx, const size_t original_length, const String& replacement) {
        if (idx >= m_uSize) {
            throw std::out_of_range("Index out of range!");
        }
        if (idx + original_length > m_uSize) {
            throw std::out_of_range("Length is out of range!");
        }
        if (original_length <= 0) {
            throw std::out_of_range("Invalid original length!");
        }
        remove(idx, original_length);
        if (!replacement.is_empty()) {
            insert(idx, replacement);
        }
        return *this;
    }
    String& String::replace(const char original, const char replacement, const size_t offset) {
        if (original != replacement) {
            if (const auto idx = find(original, offset); idx > -1) {
                m_pData[idx] = replacement;
            }
        }
        return *this;
    }
    String& String::replace(const String& original, const String& replacement, const size_t offset) {
        if (original != replacement) {
            if (const auto idx = find(original, offset); idx > -1) {
                return replace(static_cast<size_t>(idx), original.m_uSize, replacement);
            }
        }
        return *this;
    }

    String& String::replace_all(const char original, const char replacement, size_t offset) {
        if (original != replacement) {
            auto idx = find(original, offset);
            while (idx > -1) {
                m_pData[idx] = replacement;
                idx = find(original, ++offset);
            }
        }
        return *this;
    }
    String& String::replace_all(const String& original, const String& replacement, size_t offset) {
        if (original != replacement) {
            auto idx = find(original, offset);
            while (idx > -1) {
                replace(idx, original.m_uSize, replacement);
                offset += replacement.m_uSize;
                idx = find(original, offset);
            }
        }
        return *this;
    }

    size_t String::count(const char c, const size_t offset) const {
        size_t count = 0;
        if (m_pData != nullptr) {
            for (size_t i = offset; i < m_uSize; ++i) {
                if (m_pData[i] == c) {
                    ++count;
                }
            }
        }
        return count;
    }

    ssize_t String::find(const char c, const size_t offset) const {
        if (offset >= m_uSize || m_pData == nullptr) {
            return -1;
        }
        const char* p = strchr(m_pData + offset, c);
        if (p == nullptr) {
            return -1;
        }
        return p - m_pData;
    }
    ssize_t String::find(const String& str, const size_t offset) const {
        if (offset > m_uSize - str.m_uSize || m_pData == nullptr) {
            return -1;
        }
        const char* p = strstr(m_pData + offset, str.m_pData);
        if (p == nullptr) {
            return -1;
        }
        return p - m_pData;
    }
    ssize_t String::find_last(const char c, const size_t offset) const {
        if (offset >= m_uSize || m_pData == nullptr) {
            return -1;
        }
        for (auto i = static_cast<ssize_t>(m_uSize - offset); i >= 0; i--) {
            if (m_pData[i] == c) {
                return i;
            }
        }
        return -1;
    }
    ssize_t String::find_last(const String& str, const size_t offset) const {
        if (offset >= m_uSize - str.m_uSize || m_pData == nullptr) {
            return -1;
        }
        for (auto i = static_cast<ssize_t>(m_uSize - str.m_uSize - offset); i >= 0; i--) {
            if (strncmp(m_pData + i, str.m_pData, str.m_uSize) == 0) {
                return i;
            }
        }
        return -1;
    }
    ssize_t String::find_any_first_of(const String& chars, const size_t offset) const {
        if (offset >= m_uSize || m_pData == nullptr) {
            return -1;
        }
        for (auto i = static_cast<ssize_t>(offset); i < m_uSize; i++) {
            if (strchr(chars.m_pData, m_pData[i]) != nullptr) {
                return i;
            }
        }
        return -1;
    }
    ssize_t String::find_any_last_of(const String& chars, const size_t offset) const {
        if (offset >= m_uSize || m_pData == nullptr) {
            return -1;
        }
        for (auto i = static_cast<ssize_t>(m_uSize - offset - 1); i >= 0; i--) {
            if (strchr(chars.m_pData, m_pData[i]) != nullptr) {
                return i;
            }
        }
        return -1;
    }
    ssize_t String::find_any_first_not_of(const String& chars, const size_t offset) const {
        if (offset >= m_uSize || m_pData == nullptr) {
            return -1;
        }
        for (auto i = static_cast<ssize_t>(offset); i < m_uSize; i++) {
            if (strchr(chars.m_pData, m_pData[i]) == nullptr) {
                return i;
            }
        }
        return -1;
    }
    ssize_t String::find_any_last_not_of(const String& chars, const size_t offset) const {
        if (offset >= m_uSize || m_pData == nullptr) {
            return -1;
        }
        for (auto i = static_cast<ssize_t>(m_uSize - offset); i >= 0; i--) {
            if (strchr(chars.m_pData, m_pData[i]) == nullptr) {
                return i;
            }
        }
        return -1;
    }

    bool String::starts_with(const char c, const StringCompareType compare_type) const {
        if (m_pData == nullptr) {
            return false;
        }
        if (compare_type == StringCompareType::CaseSensitive) {
            return c == m_pData[0];
        }
        return std::tolower(c) == std::tolower(m_pData[0]);
    }
    bool String::starts_with(const String& str, const StringCompareType compare_type) const {
        if (m_pData == nullptr) {
            return false;
        }
        return (compare_type == StringCompareType::CaseSensitive ?
                   strncmp(m_pData, str.m_pData, str.m_uSize) :
                   strncasecmp(m_pData, str.m_pData, str.m_uSize)) == 0;
    }
    bool String::ends_with(const char c, const StringCompareType compare_type) const {
        if (m_pData == nullptr) {
            return false;
        }
        if (compare_type == StringCompareType::CaseSensitive) {
            return c == m_pData[m_uSize - 1];
        }
        return std::tolower(c) == std::tolower(m_pData[m_uSize - 1]);
    }
    bool String::ends_with(const String& str, const StringCompareType compare_type) const {
        if (m_pData == nullptr) {
            return false;
        }
        return (compare_type == StringCompareType::CaseSensitive ?
                   strncmp(m_pData + m_uSize - str.m_uSize, str.m_pData, str.m_uSize) :
                   strncasecmp(m_pData + m_uSize - str.m_uSize, str.m_pData, str.m_uSize)) == 0;
    }

    String& String::trim_begin(const String& chars) {
        if (const auto idx = find_any_first_not_of(chars); idx > -1) {
            return remove(0, idx);
        }
        return clear();
    }
    String& String::trim_end(const String& chars) {
        if (const auto idx = find_any_last_not_of(chars); idx > -1) {
            return remove_to_end(idx + 1);
        }
        return clear();
    }
    String& String::trim(const String& chars) {
        return trim_begin(chars).trim_end(chars);
    }

    String& String::to_lowercase() {
        if (m_pData != nullptr) {
            for (size_t i = 0; i < m_uSize; i++) {
                m_pData[i] = static_cast<char>(std::tolower(m_pData[i]));

            }
        }
        return *this;
    }
    String& String::to_uppercase() {
        if (m_pData != nullptr) {
            for (size_t i = 0; i < m_uSize; i++) {
                m_pData[i] = static_cast<char>(std::toupper(m_pData[i]));
            }
        }
        return *this;
    }

    String String::clone_begin_trimmed(const String& chars) const {
        String str = *this;
        return str.trim_begin(chars);
    }
    String String::clone_end_trimmed(const String& chars) const {
        String str = *this;
        return str.trim_end(chars);
    }
    String String::clone_trimmed(const String& chars) const {
        String str = *this;
        return str.trim(chars);
    }
    String String::clone_lowercase() const {
        String str = *this;
        return str.to_lowercase();
    }
    String String::clone_uppercase() const {
        String str = *this;
        return str.to_uppercase();
    }

    Vector<String> String::split() const {
        Vector<String> result;
        std::istringstream ss(as_std_str());
        std::string line;
        while (std::getline(ss, line)) {
            result.emplace_back(line);
        }
        return result;
    }
    Vector<String> String::split(const char c) const {
        Vector<String> result;
        std::istringstream ss(as_std_str());
        std::string line;
        while (std::getline(ss, line, c)) {
            result.emplace_back(line);
        }
        return result;
    }

    String String::Join(const Vector<String>& tokens, const char delim) {
        std::ostringstream ss;
        for (const auto& token : tokens) {
            if (ss.tellp() != 0) {
                ss << delim;
            }
            ss << token.as_std_str();
        }
        return String { ss.str() };
    }
    String String::Join(const Vector<String>& tokens, const String& delim) {
        std::ostringstream ss;
        for (const auto& token : tokens) {
            if (ss.tellp() != 0) {
                ss << delim.as_std_str();
            }
            ss << token.as_std_str();
        }
        return String { ss.str() };
    }

    bool String::operator<(const String& other) const {
        return as_std_str() < other.as_std_str();
    }
    bool String::operator>(const String& other) const {
        return as_std_str() > other.as_std_str();
    }

    String::iterator String::begin() {
        return iterator { *this, 0 };
    }
    String::const_iterator String::begin() const {
        return const_iterator { *this, 0 };
    }
    String::iterator String::end() {
        return iterator { *this, SIZE_MAX };
    }
    String::const_iterator String::end() const {
        return const_iterator { *this, SIZE_MAX };
    }

    const size_t String::NotFound = -1;

    StringIterator::difference_type StringIterator::operator-(const StringConstIterator& it) const {
        return static_cast<difference_type>(m_uIndex) - static_cast<difference_type>(it.m_uIndex);
    }

    bool PathMatches(const std::filesystem::path& path, const std::string& pattern) {
#ifdef _WIN32
    #error TODO
#else
        return fnmatch(pattern.c_str(), path.string().c_str(), 0) != FNM_NOMATCH;
#endif
    }

    Path::Path(const String& path) : m_sPath(path) {
#ifdef _WIN32
        m_sPath.replace_all('/', '\\');
#endif
    }

    Path::Path(String&& path) noexcept : m_sPath(std::move(path)) {
#ifdef _WIN32
        m_sPath.replace_all('/', '\\');
#endif
    }

    String Path::filename() const {
        if (const auto idx = m_sPath.find_any_last_of(FOW_PATH_SEPARATOR_ANY); idx > -1) {
            return m_sPath.substr(idx + 1);
        }
        return "";
    }
    String Path::filename_without_extension() const {
        const auto start = m_sPath.find_any_last_of(FOW_PATH_SEPARATOR_ANY);
        const auto end = m_sPath.find_last('.');
        if (start > -1 && end > -1) {
            return m_sPath.substr(start + 1, end - start - 1);
        }
        return "";
    }
    String Path::extension() const {
        if (const auto idx = m_sPath.find_last('.'); idx > -1) {
            auto ext = m_sPath.substr(idx);
            if (ext.count(FOW_PATH_SEPARATOR_CHAR) > 0
#ifdef _WIN32
                || ext.count(FOW_PATH_SEPARATOR_CHAR_ALT) > 0
#endif
            ) {
                return "";
            }
            return ext;
        }
        return "";
    }
    Path Path::parent() const {
        const auto idx = m_sPath.find_any_last_of(FOW_PATH_SEPARATOR_ANY);
        return idx > -1 ? m_sPath.substr(0, idx) : "";
    }

    std::tuple<Path, String> Path::split() const {
        return std::make_tuple(parent(), filename());
    }
    std::tuple<Path, String> Path::split_ext() const {
        return std::make_tuple(filename_without_extension(), extension());
    }

    Path Path::as_absolute() const {
        return std::filesystem::absolute(m_sPath.as_std_str());
    }
    Path Path::as_absolute(const Path& root) const {
        return std::filesystem::absolute((root / *this).m_sPath.as_std_str());
    }

    Path Path::as_relative() const {
        return std::filesystem::relative(m_sPath.as_std_str());
    }
    Path Path::as_relative(const Path& root) const {
        return std::filesystem::relative(m_sPath.as_std_str(), root.m_sPath.as_std_str());
    }

    Vector<Path> Path::list_dir() const {
        Vector<Path> result;
        for (const auto& path : std::filesystem::directory_iterator(m_sPath.as_std_str())) {
            result.emplace_back(path.path());
        }
        return result;
    }
    Vector<Path> Path::list_dir(const String& filter) const {
        Vector<Path> result;
        for (const auto& path : std::filesystem::directory_iterator(m_sPath.as_std_str())) {
            if (PathMatches(path.path(), filter.as_std_str())) {
                result.emplace_back(path.path());
            }
        }
        return result;
    }
    Vector<Path> Path::list_dir(const std::initializer_list<String>& filters) const {
        Vector<Path> result;
        for (const auto& path : std::filesystem::directory_iterator(m_sPath.as_std_str())) {
            for (const auto& filter : filters) {
                if (PathMatches(path.path(), filter.as_std_str())) {
                    result.emplace_back(path.path());
                    break;
                }
            }
        }
        return result;
    }

    Path& Path::replace_filename(const String& name) {
        m_sPath = std::move((parent() / name + extension()).m_sPath);
        return *this;
    }
    Path& Path::replace_filename_and_extension(const String& name) {
        m_sPath = std::move((parent() / name).m_sPath);
        return *this;
    }
    Path& Path::replace_extension(const String& ext) {
        m_sPath = std::move((parent() / filename_without_extension()).m_sPath);
        if (!m_sPath.ends_with('.') && !ext.starts_with('.')) {
            m_sPath += '.';
        }
        m_sPath += ext;
        return *this;
    }

    Path& Path::replace_root(const Path& new_root) {
        m_sPath = std::move((new_root / std::filesystem::relative(m_sPath.as_std_str())).m_sPath);
        return *this;
    }
    Path& Path::replace_root(const Path& old_root, const Path& new_root) {
        m_sPath = std::move((new_root / std::filesystem::relative(m_sPath.as_std_str(), old_root.m_sPath.as_std_str())).m_sPath);
        return *this;
    }

    Path& Path::to_absolute() {
        m_sPath = std::filesystem::absolute(m_sPath.as_std_str());
        return *this;
    }

    Path& Path::to_relative() {
        m_sPath = std::filesystem::relative(m_sPath.as_std_str());
        return *this;
    }
    Path& Path::to_relative(const Path& root) {
        m_sPath = std::filesystem::relative(m_sPath.as_std_str(), root.m_sPath.as_std_str());
        return *this;
    }

    Path& Path::go_back() {
        m_sPath = parent().m_sPath;
        return *this;
    }

    Path Path::CurrentDir() {
        return std::filesystem::current_path();
    }
    void Path::CurrentDir(const Path& path) {
        std::filesystem::current_path(path.as_std_path());
    }

    Path Path::Join(const std::initializer_list<Path>& paths) {
        Path result;
        for (const auto& path : paths) {
            if (result.is_empty()) {
                result = path;
            } else {
                result /= path;
            }
        }
        return result;
    }

    bool Path::CreateDirectory(const Path& path, const bool recursive) {
        try {
            if (recursive) {
                return std::filesystem::create_directories(path.as_cstr());
            }
            return std::filesystem::create_directory(path.as_cstr());
        } catch (...) {
            return false;
        }
    }
    bool Path::Remove(const Path& path, const bool recursive) {
        try {
            if (recursive) {
                return std::filesystem::remove_all(path.as_cstr());
            }
            return std::filesystem::remove(path.as_cstr());
        } catch (...) {
            return false;
        }
    }

    bool Path::Copy(const Path& src, const Path& dest) {
        try {
            std::filesystem::copy(src.as_cstr(), dest.as_cstr());
        } catch (...) {
            return false;
        }
        return true;
    }
    bool Path::Move(const Path& src, const Path& dest) {
        try {
            std::filesystem::rename(src.as_cstr(), dest.as_cstr());
        } catch (...) {
            return false;
        }
        return true;
    }

    bool Path::exists() const {
        return std::filesystem::exists(m_sPath.as_std_str());
    }
    bool Path::is_file() const {
        return std::filesystem::is_regular_file(m_sPath.as_std_str());
    }
    bool Path::is_directory() const {
        return std::filesystem::is_directory(m_sPath.as_std_str());
    }
    bool Path::is_absolute() const {
        return std::filesystem::path(m_sPath.as_std_str()).is_absolute();
    }
    bool Path::matches(const String& pattern) const {
#ifdef _WIN32
        #error TODO
#else
        return fnmatch(pattern.as_cstr(), m_sPath.as_cstr(), 0) != FNM_NOMATCH;
#endif
    }

    Path Path::operator+ (const String& other) const {
        return Path { m_sPath + other };
    }

    Path Path::operator/(const char* other) const {
        Path path = *this;
        path /= Path { other };
        return std::move(path);
    }

    Path Path::operator/(const String& other) const {
        Path path = *this;
        path /= Path { other };
        return std::move(path);
    }

    Path Path::operator/ (const Path& other) const {
        Path path = *this;
        path /= other;
        return std::move(path);
    }

    Path& Path::operator+=(const String& other) {
        m_sPath += other;
        return *this;
    }

    Path& Path::operator/=(const char* other) {
        return operator/=(Path { other });
    }

    Path& Path::operator/=(const String& other) {
        return operator/=(Path { other });
    }

    Path& Path::operator/=(const Path& other) {
        if (!m_sPath.ends_with(FOW_PATH_SEPARATOR_CHAR) && !other.m_sPath.starts_with(FOW_PATH_SEPARATOR_CHAR)) {
            m_sPath.append(FOW_PATH_SEPARATOR_CHAR);
        }
        m_sPath.append(other.m_sPath);
        return *this;
    }

    Result<bool> StringToBool(const String& str) {
        if (str.equals_any({ "true", "false" }, StringCompareType::CaseInsensitive)) {
            return str.equals("true", StringCompareType::CaseInsensitive);
        }

        if (const auto result = StringToInt<uint8_t>(str); result.has_value()) {
            return Success<bool>(result.value() != 0);
        }
        return Failure(std::format("Failed to parse string \"{}\" to boolean: Invalid format!", str));
    }

    Result<glm::vec2> StringToVec2(const String& str) {
        const auto tokens = str.split(',');
        if (tokens.size() < 2) {
            return Failure(std::format("Failed to parse string \"{}\" to vec2: Not enough tokens!", str));
        }
        try {
            return Success<glm::vec2>(glm::vec2 {
                std::stof(tokens.at(0).as_std_str()),
                std::stof(tokens.at(1).as_std_str())
            });
        } catch (const std::exception& ex) {
            return Failure(std::format("Failed to parse string \"{}\" to vec2: {}", str, ex.what()));
        }
    }
    Result<glm::vec3> StringToVec3(const String& str) {
        const auto tokens = str.split(',');
        if (tokens.size() < 3) {
            return Failure(std::format("Failed to parse string \"{}\" to vec3: Not enough tokens!", str));
        }
        try {
            return Success<glm::vec3>(glm::vec3 {
                std::stof(tokens.at(0).as_std_str()),
                std::stof(tokens.at(1).as_std_str()),
                std::stof(tokens.at(2).as_std_str())
            });
        } catch (const std::exception& ex) {
            return Failure(std::format("Failed to parse string \"{}\" to vec3: {}", str, ex.what()));
        }
    }
    Result<glm::vec4> StringToVec4(const String& str) {
        const auto tokens = str.split(',');
        if (tokens.size() < 4) {
            return Failure(std::format("Failed to parse string \"{}\" to vec4: Not enough tokens!", str));
        }
        try {
            return Success<glm::vec4>(glm::vec4 {
                std::stof(tokens.at(0).as_std_str()),
                std::stof(tokens.at(1).as_std_str()),
                std::stof(tokens.at(2).as_std_str()),
                std::stof(tokens.at(3).as_std_str())
            });
        } catch (const std::exception& ex) {
            return Failure(std::format("Failed to parse string \"{}\" to vec4: {}", str, ex.what()));
        }
    }

    Result<glm::bvec2> StringToBVec2(const String& str) {
        const auto tokens = str.split(',');
        if (tokens.size() < 2) {
            return Failure(std::format("Failed to parse string \"{}\" to bvec2: Not enough tokens!", str));
        }
        try {
            const auto x = StringToBool(tokens.at(0).as_std_str());
            const auto y = StringToBool(tokens.at(1).as_std_str());
            if (x.has_value() && y.has_value()) {
                return Success<glm::bvec2>(glm::bvec2 {
                    x.value(),
                    y.value()
                });
            } else {
                return Failure(std::format("Failed to parse string \"{}\" to bvec2: Invalid format", str));
            }
        } catch (const std::exception& ex) {
            return Failure(std::format("Failed to parse string \"{}\" to bvec2: {}", str, ex.what()));
        }
    }
    Result<glm::bvec3> StringToBVec3(const String& str) {
        const auto tokens = str.split(',');
        if (tokens.size() < 3) {
            return Failure(std::format("Failed to parse string \"{}\" to bvec3: Not enough tokens!", str));
        }
        try {
            const auto x = StringToBool(tokens.at(0).as_std_str());
            const auto y = StringToBool(tokens.at(1).as_std_str());
            const auto z = StringToBool(tokens.at(2).as_std_str());
            if (x.has_value() && y.has_value()) {
                return Success<glm::bvec3>(glm::bvec3 {
                    x.value(),
                    y.value(),
                    z.value()
                });
            } else {
                return Failure(std::format("Failed to parse string \"{}\" to bvec3: Invalid format", str));
            }
        } catch (const std::exception& ex) {
            return Failure(std::format("Failed to parse string \"{}\" to bvec3: {}", str, ex.what()));
        }
    }
    Result<glm::bvec4> StringToBVec4(const String& str) {
        const auto tokens = str.split(',');
        if (tokens.size() < 4) {
            return Failure(std::format("Failed to parse string \"{}\" to bvec4: Not enough tokens!", str));
        }
        try {
            const auto x = StringToBool(tokens.at(0).as_std_str());
            const auto y = StringToBool(tokens.at(1).as_std_str());
            const auto z = StringToBool(tokens.at(2).as_std_str());
            const auto w = StringToBool(tokens.at(3).as_std_str());
            if (x.has_value() && y.has_value()) {
                return Success<glm::bvec4>(glm::bvec4 {
                    x.value(),
                    y.value(),
                    z.value(),
                    w.value()
                });
            } else {
                return Failure(std::format("Failed to parse string \"{}\" to bvec4: Invalid format", str));
            }
        } catch (const std::exception& ex) {
            return Failure(std::format("Failed to parse string \"{}\" to bvec4: {}", str, ex.what()));
        }
    }
    Result<glm::uvec2> StringToUVec2(const String& str) {
        const auto tokens = str.split(',');
        if (tokens.size() < 2) {
            return Failure(std::format("Failed to parse string \"{}\" to uvec2: Not enough tokens!", str));
        }
        try {
            return Success<glm::uvec2>(glm::uvec2 {
                std::stoul(tokens.at(0).as_std_str()),
                std::stoul(tokens.at(1).as_std_str())
            });
        } catch (const std::exception& ex) {
            return Failure(std::format("Failed to parse string \"{}\" to uvec2: {}", str, ex.what()));
        }
    }
    Result<glm::uvec3> StringToUVec3(const String& str) {
        const auto tokens = str.split(',');
        if (tokens.size() < 3) {
            return Failure(std::format("Failed to parse string \"{}\" to uvec3: Not enough tokens!", str));
        }
        try {
            return Success<glm::uvec3>(glm::uvec3 {
                std::stoul(tokens.at(0).as_std_str()),
                std::stoul(tokens.at(1).as_std_str()),
                std::stoul(tokens.at(2).as_std_str())
            });
        } catch (const std::exception& ex) {
            return Failure(std::format("Failed to parse string \"{}\" to uvec3: {}", str, ex.what()));
        }
    }
    Result<glm::uvec4> StringToUVec4(const String& str) {
        const auto tokens = str.split(',');
        if (tokens.size() < 4) {
            return Failure(std::format("Failed to parse string \"{}\" to uvec4: Not enough tokens!", str));
        }
        try {
            return Success<glm::uvec4>(glm::uvec4 {
                std::stoul(tokens.at(0).as_std_str()),
                std::stoul(tokens.at(1).as_std_str()),
                std::stoul(tokens.at(2).as_std_str()),
                std::stoul(tokens.at(3).as_std_str())
            });
        } catch (const std::exception& ex) {
            return Failure(std::format("Failed to parse string \"{}\" to uvec4: {}", str, ex.what()));
        }
    }

    Result<glm::ivec2> StringToIVec2(const String& str) {
        const auto tokens = str.split(',');
        if (tokens.size() < 2) {
            return Failure(std::format("Failed to parse string \"{}\" to ivec2: Not enough tokens!", str));
        }
        try {
            return Success<glm::ivec2>(glm::ivec2 {
                std::stoi(tokens.at(0).as_std_str()),
                std::stoi(tokens.at(1).as_std_str())
            });
        } catch (const std::exception& ex) {
            return Failure(std::format("Failed to parse string \"{}\" to ivec2: {}", str, ex.what()));
        }
    }
    Result<glm::ivec3> StringToIVec3(const String& str) {
        const auto tokens = str.split(',');
        if (tokens.size() < 3) {
            return Failure(std::format("Failed to parse string \"{}\" to ivec3: Not enough tokens!", str));
        }
        try {
            return Success<glm::ivec3>(glm::ivec3 {
                std::stoi(tokens.at(0).as_std_str()),
                std::stoi(tokens.at(1).as_std_str()),
                std::stoi(tokens.at(2).as_std_str())
            });
        } catch (const std::exception& ex) {
            return Failure(std::format("Failed to parse string \"{}\" to ivec3: {}", str, ex.what()));
        }
    }
    Result<glm::ivec4> StringToIVec4(const String& str) {
        const auto tokens = str.split(',');
        if (tokens.size() < 4) {
            return Failure(std::format("Failed to parse string \"{}\" to ivec4: Not enough tokens!", str));
        }
        try {
            return Success<glm::ivec4>(glm::ivec4 {
                std::stoi(tokens.at(0).as_std_str()),
                std::stoi(tokens.at(1).as_std_str()),
                std::stoi(tokens.at(2).as_std_str()),
                std::stoi(tokens.at(3).as_std_str())
            });
        } catch (const std::exception& ex) {
            return Failure(std::format("Failed to parse string \"{}\" to ivec4: {}", str, ex.what()));
        }
    }

    Result<glm::dvec2> StringToDVec2(const String& str) {
        const auto tokens = str.split(',');
        if (tokens.size() < 2) {
            return Failure(std::format("Failed to parse string \"{}\" to dvec2: Not enough tokens!", str));
        }
        try {
            return Success<glm::dvec2>(glm::vec2 {
                std::stod(tokens.at(0).as_std_str()),
                std::stod(tokens.at(1).as_std_str())
            });
        } catch (const std::exception& ex) {
            return Failure(std::format("Failed to parse string \"{}\" to dvec2: {}", str, ex.what()));
        }
    }
    Result<glm::dvec3> StringToDVec3(const String& str) {
        const auto tokens = str.split(',');
        if (tokens.size() < 4) {
            return Failure(std::format("Failed to parse string \"{}\" to dvec3: Not enough tokens!", str));
        }
        try {
            return Success<glm::dvec3>(glm::dvec3 {
                std::stod(tokens.at(0).as_std_str()),
                std::stod(tokens.at(1).as_std_str()),
                std::stod(tokens.at(2).as_std_str())
            });
        } catch (const std::exception& ex) {
            return Failure(std::format("Failed to parse string \"{}\" to dvec3: {}", str, ex.what()));
        }
    }
    Result<glm::dvec4> StringToDVec4(const String& str) {
        const auto tokens = str.split(',');
        if (tokens.size() < 4) {
            return Failure(std::format("Failed to parse string \"{}\" to dvec4: Not enough tokens!", str));
        }
        try {
            return Success<glm::dvec4>(glm::dvec4 {
                std::stod(tokens.at(0).as_std_str()),
                std::stod(tokens.at(1).as_std_str()),
                std::stod(tokens.at(2).as_std_str()),
                std::stod(tokens.at(3).as_std_str())
            });
        } catch (const std::exception& ex) {
            return Failure(std::format("Failed to parse string \"{}\" to dvec4: {}", str, ex.what()));
        }
    }

    Result<glm::mat4> StringToMat4(const String& str) {
        const auto tokens = str.split(',');
        if (tokens.size() < 16) {
            return Failure(std::format("Failed to parse string \"{}\" to mat4: Not enough tokens!", str));
        }

        try {
            glm::mat4 result;
            for (int i = 0; i < 16; ++i) {
                result[i / 4][i % 4] = std::stof(tokens.at(i).as_std_str());
            }
            return Success<glm::mat4>(result);
        } catch (const std::exception& ex) {
            return Failure(std::format("Failed to parse string \"{}\" to mat4: {}", str, ex.what()));
        }
    }

    std::ostream& operator<<(std::ostream& os, const String& str) {
        os << str.as_std_str();
        return os;
    }
    std::istream& operator>>(std::istream& is, String& str) {
        std::string sstr;
        is >> sstr;
        str = sstr;
        return is;
    }

    std::ostream& operator<<(std::ostream& os, const Path& path) {
        os << path.m_sPath;
        return os;
    }
    std::istream& operator>>(std::istream& is, Path& path) {
        is >> path.m_sPath;
        return is;
    }
}
