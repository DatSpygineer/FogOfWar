#ifndef FOW_DYLIB_HPP
#define FOW_DYLIB_HPP

#include "fow/Shared/Api.hpp"
#include "fow/Shared/String.hpp"

namespace fow {
    class FOW_SHARED_API Dylib {
        void* m_pLibrary;
    public:
        explicit Dylib(const Path& path);
        ~Dylib();

        void* symbol(const String& name) const;
        template<typename T>
        inline T symbol(const String& name) const {
            return reinterpret_cast<T>(symbol(name));
        }

        [[nodiscard]] constexpr bool is_valid() const { return m_pLibrary != nullptr; }

        [[nodiscard]] inline void* operator[] (const String& name) const {
            return symbol(name);
        }
    };
}

#endif