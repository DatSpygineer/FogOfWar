#include "fow/Shared/Dylib.hpp"

#ifdef _WIN32
    #define WIN32_LEAN_AND_MEAN
    #include <Windows.h>
#else
    #include <dlfcn.h>
#endif

namespace fow {
    Dylib::Dylib(const Path& path) {
#ifdef _WIN32
    #error TODO
#else
        m_pLibrary = dlopen(path.as_cstr(), RTLD_LAZY);
#endif
    }
    Dylib::~Dylib() {
        if (m_pLibrary != nullptr) {
#ifdef _WIN32
    #error TODO
#else
            dlclose(m_pLibrary);
#endif
        }
    }

    void* Dylib::symbol(const String& name) const {
        if (m_pLibrary == nullptr) {
            return nullptr;
        }
#ifdef _WIN32
    #error TODO
#else
        return dlsym(m_pLibrary, name.as_cstr());
#endif
    }
}
