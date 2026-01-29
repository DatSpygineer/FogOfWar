#ifdef _WIN32

#include <Shlwapi.h>

bool Win32PathSpec(const char* path, const char* pattern) {
    return PathMatchSpecA(path, pattern);
}

#endif