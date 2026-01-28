#ifndef FOW_POINTER_HELPER_HPP
#define FOW_POINTER_HELPER_HPP

#include <cstddef>
#include <cstring>

namespace fow {
    template<typename T>
    T* CopyPtr(T* trg, const T* src, const size_t count) {
        return static_cast<T*>(memcpy(static_cast<void*>(trg), static_cast<const void*>(src), count * sizeof(T)));
    }
    template<typename T>
    T* MovePtr(T* trg, const T* src, const size_t count) {
        return static_cast<T*>(memmove(static_cast<void*>(trg), static_cast<const void*>(src), count * sizeof(T)));
    }
    template<typename T>
    T* ResizePtr(T* ptr, const size_t old_size, const size_t new_size) {
        T* new_ptr = new T[new_size];
        CopyPtr(new_ptr, ptr, std::min(old_size, new_size));
        delete[] ptr;
        return new_ptr;
    }
}

#endif