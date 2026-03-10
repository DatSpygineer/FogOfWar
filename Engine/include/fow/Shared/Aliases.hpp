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
    using SharedPtr = std::shared_ptr<T>;
    template<typename T>
    using UniquePtr = std::unique_ptr<T>;
}

#endif