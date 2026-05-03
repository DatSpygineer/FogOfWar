#ifndef FOW_COLLECTIONS_HPP
#define FOW_COLLECTIONS_HPP

#include "fow/Shared/Api.hpp"
#include "fow/Shared/Aliases.hpp"

namespace fow {
    namespace Collections {
        template<typename T>
        inline Vector<T> CopySlice(const Vector<T>& in, const size_t start, const size_t length) {
            if (start >= in.size()) {
                return { };
            }
            Vector<T> result(length);
            std::copy(in.begin() + start, in.begin() + start + length, result.begin());
            return result;
        }
        template<typename T>
        inline Vector<T> CopySlice(const Vector<T>& in, const size_t start) {
            if (start >= in.size()) {
                return { };
            }
            Vector<T> result(in.size() - start);
            std::copy(in.begin() + start, in.end(), result.begin());
            return result;
        }
        template<typename T>
        inline Vector<T> JoinVectors(const InitList<Vector<T>>& vectors) {
            Vector<T> result;
            size_t size = 0;
            for (const auto& vec : vectors) {
                size += vec.size();
            }
            result.reserve(size);

            for (const auto& vec : vectors) {
                result.insert(result.end(), vec.begin(), vec.end());
            }

            return result;
        }
    }
}

#endif