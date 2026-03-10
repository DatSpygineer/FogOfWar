#ifndef FOW_ALGO_HPP
#define FOW_ALGO_HPP

#include "Aliases.hpp"
#include "Result.hpp"

namespace fow {
    template<typename TKey, typename TResult>
    inline Option<TResult> Match(const TKey& key, const HashMap<TKey, TResult>& values) {
        if (values.contains(key)) {
            return Some(values.at(key));
        }
        return None();
    }
    template<typename TKey, typename TResult>
    inline Option<TResult> Match(const TKey& key, const HashMap<TKey, Function<TResult>>& values) {
        if (values.contains(key)) {
            return Some(values.at(key)());
        }
        return None();
    }
    template<typename TKey, typename TResult>
    inline TResult Match(const TKey& key, const TResult& default_value, const HashMap<TKey, TResult>& values) {
        if (values.contains(key)) {
            return values.at(key);
        }
        return default_value;
    }
    template<typename TKey, typename TResult>
    inline TResult Match(const TKey& key, const Function<TResult>& default_value, const HashMap<TKey, TResult>& values) {
        if (values.contains(key)) {
            return values.at(key);
        }
        return default_value();
    }
    template<typename TKey, typename TResult>
    inline TResult Match(const TKey& key, const TResult& default_value, const HashMap<TKey, Function<TResult>>& values) {
        if (values.contains(key)) {
            return values.at(key)();
        }
        return default_value;
    }
    template<typename TKey, typename TResult>
    inline TResult Match(const TKey& key, const Function<TResult>& default_value, const HashMap<TKey, Function<TResult>>& values) {
        if (values.contains(key)) {
            return values.at(key)();
        }
        return default_value();
    }
}

#endif
