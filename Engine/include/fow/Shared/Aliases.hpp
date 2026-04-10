#ifndef FOW_ALIASES_HPP
#define FOW_ALIASES_HPP

#include <chrono>
#include <memory>
#include <deque>
#include <vector>
#include <functional>
#include <unordered_map>
#include <map>

namespace fow {
    namespace Time {
        using namespace std::chrono;
        using TimePoint = system_clock::time_point;
        inline auto Now() {
            return zoned_time(current_zone(), system_clock::now());
        }
    }

    template<typename T>
    using Vector = std::vector<T>;
    template<typename T>
    using Deque = std::deque<T>;
    template<typename T>
    using InitList = std::initializer_list<T>;
    template<typename K, typename V>
    using HashMap = std::unordered_map<K, V>;
    template<typename K, typename V>
    using SortedMap = std::map<K, V>;

    template<typename Signature>
    using Function = std::function<Signature>;

    template<typename T>
    using Ref = std::shared_ptr<T>;
    template<typename T, typename... Args>
    inline Ref<T> CreateRef(Args&&... args) {
        return std::make_shared<T>(std::forward<Args>(args)...);
    }
    template<typename TTo, typename TFrom>
    inline Ref<TTo> CastRef(const Ref<TFrom>& t) {
        return std::dynamic_pointer_cast<TTo>(t);
    }
    template<typename T>
    using UniquePtr = std::unique_ptr<T>;

#if defined(_WIN32) && (defined(_UNICODE) || defined(UNICODE))
    #define FOW_OS_USE_WCHAR 1
    typedef wchar_t os_char_t;
#else
    #define FOW_OS_USE_WCHAR 0
    typedef char os_char_t;
#endif
}

#endif