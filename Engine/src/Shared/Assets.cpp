#include "fow/Shared/Assets.hpp"

namespace fow {
    ZipArchive::ZipArchive(const Path& archive_path) {
        int err_code;
        m_pZip = zip_open(archive_path.as_cstr(), ZIP_RDONLY, &err_code);
        if (m_pZip == nullptr) {
            zip_error_t error;
            zip_error_init_with_code(&error, err_code);
            auto e = std::runtime_error(std::format("Failed to open zip file \"{}\": {}", archive_path, zip_error_strerror(&error)));
            zip_error_fini(&error);
            throw e;
        }
    }
    ZipArchive::~ZipArchive() {
        if (m_pZip != nullptr) {
            zip_close(m_pZip);
            m_pZip = nullptr;
        }
    }

    bool ZipArchive::has_entry(const Path& path) const {
        auto fixed_path = path;
        if (path.as_string().starts_with('/')) {
            fixed_path = path.as_string().substr(1);
        }
        if (const auto zip = zip_fopen(m_pZip, fixed_path.as_cstr(), 0); zip != nullptr) {
            zip_fclose(zip);
            return true;
        }
        return false;
    }
    Result<UniquePtr<ZipEntry>> ZipArchive::open_entry(const Path& path) const {
        try {
            auto fixed_path = path;
            if (path.as_string().starts_with('/')) {
                fixed_path = path.as_string().substr(1);
            }
            return Success<UniquePtr<ZipEntry>>(std::make_unique<ZipEntry>(*this, fixed_path));
        } catch (const std::runtime_error& e) {
            return Failure<UniquePtr<ZipEntry>>(e.what());
        }
    }

    ZipEntry::ZipEntry(const ZipArchive& archive, const Path& path) : m_refArchive(archive), m_sPath(path) {
        m_pZipFile = zip_fopen(archive.m_pZip, path.as_cstr(), 0);
        if (m_pZipFile == nullptr) {
            zip_error_t* error = zip_get_error(archive.m_pZip);
            auto e = std::runtime_error(std::format("Failed to open zip file entry \"{}\": {}", path, zip_error_strerror(error)));
            zip_error_fini(error);
            throw e;
        }
    }
    ZipEntry::~ZipEntry() {
        if (m_pZipFile != nullptr) {
            zip_fclose(m_pZipFile);
            m_pZipFile = nullptr;
        }
    }

    uint64_t ZipEntry::size() const {
        struct zip_stat stat;
        if (zip_stat(m_refArchive.m_pZip, m_sPath.as_cstr(), 0, &stat) > -1) {
            return stat.size;
        }
        return 0;
    }
    int64_t ZipEntry::read(void* buffer, const uint64_t read_size) const {
        if (m_pZipFile == nullptr) {
            return -1;
        }
        return zip_fread(m_pZipFile, buffer, read_size);
    }
    int64_t ZipEntry::read_to_end(void* buffer, const uint64_t max_buffer_size) const {
        return read(buffer, std::min(size(), max_buffer_size));
    }

    String ZipEntry::read_string() const {
        const size_t read_size = size();
        String result(read_size);
        read(result.data(), read_size);
        result.data()[read_size] = '\0';
        result.recalculate_size();
        return result;
    }

    namespace Assets {
        static Path           s_asset_base_path = "";
        static Option<Path>   s_mod_base_path   = None();
        static Vector<String> s_archive_names;
        static HashMap<Path, std::any> s_asset_cache;

        bool Initialize(const Path& asset_base_dir, const Vector<String>& archive_names, const Option<Path>& mod_base_dir) {
            if (!s_asset_base_path.is_empty()) {
                return false;
            }

            s_asset_base_path = asset_base_dir;
            s_mod_base_path = mod_base_dir;
            s_archive_names = archive_names;
            return true;
        }
        Path GetBaseDataPath() {
            return s_asset_base_path;
        }
        Option<Path> GetBaseModPath() {
            return s_mod_base_path;
        }

        const Vector<String>& GetMainArchiveNames() {
            return s_archive_names;
        }

        HashMap<String, Vector<String>> ListAssets() {
            if (GetBaseDataPath().is_empty()) {
                throw std::invalid_argument(std::format("Failed to list assets: Base data path is not set! Assets::Initialize must be run before attempting to load an asset!"));
            }

            HashMap<String, Vector<String>> result;
            Vector<String> temp;

            if (const auto mod_base = GetBaseModPath(); mod_base.has_value()) {
                if (mod_base->exists()) {
                    auto modlist_path = mod_base.value() / "mods.txt"_s;
                    auto modlist = Vector<String>();
                    if (modlist_path.exists()) {
                        std::ifstream ifs(modlist_path.as_std_path());
                        std::string line;
                        while (std::getline(ifs, line)) {
                            modlist.emplace_back(line);
                        }
                        ifs.close();
                    }

                    try {
                        for (const auto mod : modlist) {
                            const auto archive_path = mod_base.value() / mod;
                            if (zip_t* zip = zip_open(archive_path.as_cstr(), 0, nullptr); zip != nullptr) {
                                const int64_t count = zip_get_num_entries(zip, 0);
                                for (auto i = 0; i < count; ++i) {
                                    temp.emplace_back(zip_get_name(zip, i, 0));
                                }
                                zip_close(zip);
                                result.emplace("mod/"_s + mod, temp);
                                temp.clear();
                            }
                        }
                    } catch (const std::exception&) {
                    }
                } else {
                    Path::CreateDirectory(mod_base.value(), true);
                }
            }

            try {
                for (const auto archive_name : GetMainArchiveNames()) {
                    const auto archive_path = GetBaseDataPath() / archive_name;
                    if (zip_t* zip = zip_open(archive_path.as_cstr(), 0, nullptr); zip != nullptr) {
                        const int64_t count = zip_get_num_entries(zip, 0);
                        for (auto i = 0; i < count; ++i) {
                            temp.emplace_back(zip_get_name(zip, i, 0));
                        }
                        zip_close(zip);
                        result.emplace("data/"_s + archive_name, temp);
                        temp.clear();
                    }
                }
            } catch (const std::exception& e) {
            }
            return result;
        }

        Vector<String> GetModLoadOrder() {
            Vector<String> result;
            if (const auto mod_dir = GetBaseModPath(); mod_dir.has_value()) {
                if (auto mod_list = mod_dir.value() / "mods.txt"; mod_list.exists()) {
                    std::ifstream ifs(mod_list.as_std_path());
                    std::string line;
                    while (std::getline(ifs, line)) {
                        result.emplace_back(line);
                    }
                    ifs.close();
                } else {
                    if (!mod_dir.value().exists()) {
                        Path::CreateDirectory(mod_dir.value(), true);
                    }
                    std::ofstream ofs((mod_dir.value() / "mods.txt").as_std_path());
                    for (const auto& path : mod_dir.value().list_dir({ "*.zip", "*.fwpak" })) {
                        result.emplace_back(path.as_std_str());
                        ofs << path.as_std_str() << std::endl;
                    }
                    ofs.close();
                }
            }
            return result;
        }

        const std::any& CacheAsset(const Path& path, std::any&& asset) {
            if (!s_asset_cache.contains(path)) {
                s_asset_cache.emplace(path, std::move(asset));
            }
            return s_asset_cache.at(path);
        }
        const std::any& CacheAsset(const Path& path, const std::any& asset) {
            if (!s_asset_cache.contains(path)) {
                s_asset_cache.emplace(path, asset);
            }
            return s_asset_cache.at(path);
        }
        Result<std::any> GetCachedAsset(const Path& path) {
            if (s_asset_cache.contains(path)) {
                return s_asset_cache.at(path);
            }
            return Failure<std::any>(std::format("Asset \"{}\" is not in cache!", path));
        }

        bool IsCached(const Path& path) {
            return s_asset_cache.contains(path);
        }

        void ClearCache() {
            s_asset_cache.clear();
        }

        void RemoveCache(const Path& path) {
            if (s_asset_cache.contains(path)) {
                s_asset_cache.erase(path);
            }
        }

        Result<String> LoadFromArchiveAsString(const Path& archive_path, const Path& asset_path) {
            const ZipArchive* zip = new ZipArchive(archive_path);
            const auto entry_result = zip->open_entry(asset_path);
            Result<String> result;
            if (!entry_result.has_value()) {
                result = Failure<String>(std::format("Failed to load asset \"{}\": File doesn't exists", asset_path));
            } else {
                result = entry_result.value()->read_string();
            }
            delete zip;
            return result;
        }

        Result<Vector<uint8_t>> LoadFromArchiveAsBytes(const Path& archive_path, const Path& asset_path) {
            const ZipArchive* zip = new ZipArchive(archive_path);
            const auto entry_result = zip->open_entry(asset_path);
            Result<Vector<uint8_t>> result;
            if (!entry_result.has_value()) {
                result = Failure<Vector<uint8_t>>(std::format("Failed to load asset \"{}\": File doesn't exists", asset_path));
            } else {
                Vector<uint8_t> result_value(entry_result.value()->size());
                entry_result.value()->read_to_end(result_value.data(), result_value.size());
                result = result_value;
            }
            delete zip;
            return result;
        }

        Result<pugi::xml_document> LoadFromArchiveAsXml(const Path& archive_path, const Path& asset_path) {
            const auto str = LoadFromArchiveAsString(archive_path, asset_path);
            if (!str.has_value()) {
                return Failure<pugi::xml_document>(str.error());
            }
            pugi::xml_document doc;
            if (const auto result = doc.load_string(str->as_cstr()); result.status != pugi::status_ok) {
                return Failure<pugi::xml_document>(std::format("Failed to parse xml document \"{}\": {}", asset_path, result.description()));
            }
            return Success<pugi::xml_document>(std::move(doc));
        }

        Result<nlohmann::json> LoadFromArchiveAsJson(const Path& archive_path, const Path& asset_path) {
            const auto str = LoadFromArchiveAsString(archive_path, asset_path);
            if (!str.has_value()) {
                return Failure<nlohmann::json>(str.error());
            }
            try {
                auto json = nlohmann::json::parse(str->as_std_str());
                return Success<nlohmann::json>(std::move(json));
            } catch (const std::exception& e) {
                return Failure<nlohmann::json>(e.what());
            }
        }

        Result<String> LoadAsString(const Path& asset_path, const AssetLoaderFlags::Type flags) {
            if (!(flags & AssetLoaderFlags::IgnoreMods) && GetBaseModPath().has_value()) {
                for (const auto mods = GetModLoadOrder(); const auto& mod : mods) {
                    if (const auto result = LoadFromArchiveAsString(GetBaseModPath().value() / mod, asset_path); result.has_value()) {
                        return result;
                    }
                }
            }

            for (const auto& archive : GetMainArchiveNames()) {
                if (const auto result = LoadFromArchiveAsString(GetBaseDataPath() / archive, asset_path); result.has_value()) {
                    return result;
                }
            }
            return Failure<String>(std::format("Failed to load asset \"{}\": Asset cannot be found!", asset_path));
        }

        Result<Vector<uint8_t>> LoadAsBytes(const Path& asset_path, const AssetLoaderFlags::Type flags) {
            if (!(flags & AssetLoaderFlags::IgnoreMods) && GetBaseModPath().has_value()) {
                for (const auto mods = GetModLoadOrder(); const auto& mod : mods) {
                    if (const auto result = LoadFromArchiveAsBytes(GetBaseModPath().value() / mod, asset_path); result.has_value()) {
                        return result;
                    }
                }
            }

            for (const auto& archive : GetMainArchiveNames()) {
                if (const auto result = LoadFromArchiveAsBytes(GetBaseDataPath() / archive, asset_path); result.has_value()) {
                    return result;
                }
            }
            return Failure<Vector<uint8_t>>(std::format("Failed to load asset \"{}\": Asset cannot be found!", asset_path));
        }

        Result<pugi::xml_document> LoadAsXml(const Path& asset_path, AssetLoaderFlags::Type flags) {
            const auto str = LoadAsString(asset_path, flags);
            if (!str.has_value()) {
                return Failure<pugi::xml_document>(str.error());
            }
            pugi::xml_document doc;
            if (const auto result = doc.load_string(str->as_cstr()); result.status != pugi::status_ok) {
                return Failure<pugi::xml_document>(std::format("Failed to parse xml document \"{}\": {}", asset_path, result.description()));
            }
            return Success<pugi::xml_document>(std::move(doc));
        }

        Result<nlohmann::json> LoadAsJson(const Path& asset_path, AssetLoaderFlags::Type flags) {
            const auto str = LoadAsString(asset_path, flags);
            if (!str.has_value()) {
                return Failure<nlohmann::json>(str.error());
            }
            try {
                auto json = nlohmann::json::parse(str->as_std_str());
                return Success<nlohmann::json>(std::move(json));
            } catch (const std::exception& e) {
                return Failure<nlohmann::json>(e.what());
            }
        }

        Result<HashMap<String, String>> LoadStringsFromAllArchives(const Path& asset_path, AssetLoaderFlags::Type flags) {
            HashMap<String, String> result;
            if (const auto mod_basepath = GetBaseModPath(); mod_basepath.has_value()) {
                for (const auto mods = GetModLoadOrder(); const auto& mod : mods) {
                    const auto archive_path = mod_basepath.value() / mod;
                    if (const auto value = LoadFromArchiveAsString(archive_path, asset_path); value.has_value()) {
                        result.emplace(mod, value.value());
                    }
                }
            }
            for (const auto& archive_name : GetMainArchiveNames()) {
                const auto archive_path = GetBaseDataPath() / archive_name;
                if (const auto value = LoadFromArchiveAsString(archive_path, asset_path); value.has_value()) {
                    result.emplace(archive_name, value.value());
                }
            }
            return Failure<HashMap<String, String>>(std::format("Failed to load asset \"{}\": Asset cannot be found!", asset_path));
        }
    }
}
