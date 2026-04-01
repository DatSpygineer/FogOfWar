#ifndef FOW_FILESYS_HPP
#define FOW_FILESYS_HPP

#include <fow/Shared/Api.hpp>
#include <fow/Shared/Result.hpp>

namespace fow::Files {
    FOW_SHARED_API Result<String> ReadAllText(const Path& path);
    FOW_SHARED_API Result<Vector<String>> ReadAllLines(const Path& path);
    FOW_SHARED_API Result<Vector<uint8_t>> ReadAllBytes(const Path& path);

    FOW_SHARED_API Result<> WriteAllText(const Path& path, const String& text);
    FOW_SHARED_API Result<> WriteAllLines(const Path& path, const Vector<String>& lines);
    FOW_SHARED_API Result<> WriteAllBytes(const Path& path, const Vector<uint8_t>& lines);
}

#endif