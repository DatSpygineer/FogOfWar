#include "fow/Lang.hpp"
#include "fow/Assets.hpp"

#include "rfl/json.hpp"

namespace fow {
    struct Lang {
        String id;
        String display_name;
        HashMap<String, String> translations;
    };

    static HashMap<String, Lang> s_languages;
    static String s_selected_language = "en_us";

    Result<> LoadLanguageFiles() {
        const auto json = Assets::LoadAsString("Lang.json", AssetLoaderFlags::IgnoreMods);
        if (!json.has_value()) {
            return Failure(json.error());
        }

        const auto languages = rfl::json::read<HashMap<std::string, std::string>>(json.value().as_std_str());
        if (!languages.has_value()) {
            return Failure(std::format("Failed to load languages: {}", languages.error().what()));
        }

        for (const auto [ id, name ] : languages.value()) {
            if (const auto lang_data = Assets::LoadAsString(Path(std::format("/Lang/{}.json", id).c_str())); lang_data.has_value()) {
                const auto data_result = rfl::json::read<HashMap<std::string, std::string>>(lang_data.value().as_cstr());
                if (!data_result.has_value()) {
                    return Failure(std::format("Failed to load language \"{}\": {}", id, data_result.error().what()));
                }

                auto data = data_result.value();
                HashMap<String, String> translations;
                translations.reserve(data.size());

                for (const auto [ key, value ] : data) {
                    translations.emplace(key, value);
                }
                s_languages.emplace(id, Lang { id, name, std::move(translations) });
            } else {
                Debug::LogWarning(std::format("Translation \"{}\" is missing!", id));
            }
        }
        return Success();
    }

    Result<> SetLanguage(const String& lang) {
        if (s_languages.contains(lang)) {
            s_selected_language = lang;
            return Success();
        }
        return Failure(std::format("No such language \"{}\"", lang));
    }
    String GetCurrentLanguage() {
        return s_selected_language;
    }

    Vector<String> GetAvailableLanguages() {
        Vector<String> result;
        for (const auto key: s_languages | std::views::keys) {
            result.push_back(key);
        }
        return result;
    }

    String Translate(const String& key, const String& default_value) {
        if (s_languages.contains(s_selected_language) && s_languages.at(s_selected_language).translations.contains(key)) {
            return s_languages.at(s_selected_language).translations.at(key);
        }
        return default_value;
    }
}
