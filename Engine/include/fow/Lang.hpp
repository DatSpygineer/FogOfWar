#ifndef FOW_LANG_HPP
#define FOW_LANG_HPP

#include "fow/Shared.hpp"
#include "fow/String.hpp"
#include "fow/Result.hpp"

namespace fow {
    Result<> LoadLanguageFiles();

    FOW_SHARED_API Result<> SetLanguage(const String& lang);
    FOW_SHARED_API String GetCurrentLanguage();
    FOW_SHARED_API Vector<String> GetAvailableLanguages();
    FOW_SHARED_API String Translate(const String& key, const String& default_value);
    inline String Translate(const String& key) { return Translate(key, key); }

    inline String operator""_T(const char* cstr, const size_t len) {
        return Translate(String { cstr, len });
    }
}

#endif