#ifndef FOW_ASSETS_HPP
#define FOW_ASSETS_HPP

#include "fow/Shared/Api.hpp"
#include "fow/Shared/String.hpp"
#include "fow/Shared/Debug.hpp"

#include <fstream>
#include <any>
#include <zip.h>

#include <nlohmann/json.hpp>

namespace fow {
    class ZipEntry;

    class FOW_SHARED_API ZipArchive final {
        zip_t* m_pZip;
    public:
        explicit ZipArchive(const Path& archive_path);
        ~ZipArchive();

        [[nodiscard]] bool has_entry(const Path& path) const;
        [[nodiscard]] Result<UniquePtr<ZipEntry>> open_entry(const Path& path) const;

        [[nodiscard]] FOW_CONSTEXPR bool is_valid() const { return m_pZip != nullptr; }
        [[nodiscard]] Vector<String> list_assets() const;
        [[nodiscard]] int64_t entry_count() const;

        friend class ZipEntry;
    };
    class FOW_SHARED_API ZipEntry final {
        String m_sPath;
        const ZipArchive& m_refArchive;
        zip_file_t* m_pZipFile;
    public:
        ZipEntry(const ZipArchive& archive, const String& path);
        ~ZipEntry();

        [[nodiscard]] uint64_t size() const;
        int64_t read(void* buffer, uint64_t read_size) const;
        int64_t read_to_end(void* buffer, uint64_t max_buffer_size) const;
        [[nodiscard]] String read_string() const;
        [[nodiscard]] FOW_CONSTEXPR const String& path() const { return m_sPath; }
    };

    template<typename T>
    class Asset {
        Path m_sPath;
        SharedPtr<T> m_pData;
    public:
        Asset() : m_sPath(), m_pData(nullptr) { }
        Asset(const Path& path, const SharedPtr<T>& data) : m_sPath(path), m_pData(data) { }
        Asset(const Path& path, SharedPtr<T>&& data) : m_sPath(path), m_pData(std::move(data)) {
            data = nullptr;
        }
        Asset(const Asset& other) : m_sPath(other.m_sPath), m_pData(other.m_pData) { }
        Asset(Asset&& other) noexcept : m_sPath(std::move(other.m_sPath)), m_pData(std::move(other.m_pData)) {
            other.m_sPath = "";
            other.m_pData = nullptr;
        }

        Asset& operator=(const Asset& other) {
            m_sPath = other.m_sPath;
            m_pData = other.m_pData;
            return *this;
        }
        Asset& operator=(Asset&& other) noexcept {
            m_sPath = std::move(other.m_sPath);
            m_pData = std::move(other.m_pData);
            other.m_sPath = "";
            other.m_pData = nullptr;
            return *this;
        }

        [[nodiscard]] FOW_CONSTEXPR const Path& path() const { return m_sPath; }
        [[nodiscard]] FOW_CONSTEXPR bool is_valid() const { return !m_sPath.is_empty() && m_pData != nullptr; }

        [[nodiscard]] FOW_CONSTEXPR SharedPtr<T>& ptr() { return m_pData; }
        [[nodiscard]] FOW_CONSTEXPR const SharedPtr<T>& ptr() const { return m_pData; }
        [[nodiscard]] FOW_CONSTEXPR T& value() { return *m_pData.get(); }
        [[nodiscard]] FOW_CONSTEXPR const T& value() const { return *m_pData.get(); }

        [[nodiscard]] FOW_CONSTEXPR T* operator->() { return m_pData.get(); }
        [[nodiscard]] FOW_CONSTEXPR const T* operator->() const { return m_pData.get(); }
    };

    namespace AssetLoaderFlags {
        enum Type {
            Default    = 0b0000,
            IgnoreMods = 0b0001
        };
    }

    namespace Assets {
        FOW_SHARED_API Result<> Initialize(const Path& asset_base_dir, const Vector<String>& archive_names, const Option<Path>& mod_base_dir = None());
        FOW_SHARED_API Path GetBaseDataPath();
        FOW_SHARED_API Option<Path> GetBaseModPath();
        FOW_SHARED_API const Vector<String>& GetMainArchiveNames();
        FOW_SHARED_API HashMap<String, Vector<String>> ListAssets();
        FOW_SHARED_API Vector<String> ListAssets(const String& archive_name, bool is_mod);
        FOW_SHARED_API Vector<String> GetModLoadOrder();

        FOW_SHARED_API const std::any& CacheAsset(const Path& path, std::any&& asset);
        FOW_SHARED_API const std::any& CacheAsset(const Path& path, const std::any& asset);
        FOW_SHARED_API Result<std::any> GetCachedAsset(const Path& path);
        FOW_SHARED_API bool IsCached(const Path& path);
        FOW_SHARED_API void ClearCache();
        FOW_SHARED_API void RemoveCache(const Path& path);

        template<typename T>
        inline Result<Asset<T>> Load(const Path& asset_path, const AssetLoaderFlags::Type flags = AssetLoaderFlags::Default) {
            if (const auto cached = GetCachedAsset(asset_path); cached.has_value()) {
                return std::any_cast<Asset<T>>(cached.value());
            }

            if (const auto mod_basepath = GetBaseModPath(); mod_basepath.has_value() && !(flags & AssetLoaderFlags::IgnoreMods)) {
                for (const auto& mod : GetModLoadOrder()) {
                    const auto archive_path = mod_basepath.value() / mod;
                    const ZipArchive* archive = nullptr;
                    try {
                        archive = new ZipArchive(archive_path);
                    } catch (const std::runtime_error& _) {
                        Debug::LogError(std::format("Could not open archive \"{}\", please check the mod list!", archive_path));
                        continue;
                    }

                    if (archive->has_entry(asset_path)) {
                        const auto result = T::LoadAsset(asset_path, flags);
                        delete archive;
                        if (result.has_value()) {
                            try {
                                return std::any_cast<Asset<T>>(CacheAsset(asset_path, std::move(Asset<T> { asset_path, std::move(result.value()) })));
                            } catch (const std::bad_any_cast& e) {
                                return Failure(std::format("Failed to load asset \"{}\": {}", asset_path, e.what()));
                            }
                        } else {
                            return Failure(result.error());
                        }
                    } else {
                        delete archive;
                    }
                }
            }
            for (const auto& archive_name : GetMainArchiveNames()) {
                const auto archive_path = GetBaseDataPath() / archive_name;
                const ZipArchive* archive = nullptr;
                try {
                    archive = new ZipArchive(archive_path);
                } catch (const std::runtime_error& _) {
                    Debug::LogError(std::format("Could not open archive \"{}\", please check the mod list!", archive_path));
                    continue;
                }

                if (archive->has_entry(asset_path)) {
                    const auto result = T::LoadAsset(asset_path, flags);
                    delete archive;
                    if (result.has_value()) {
                        try {
                            return std::any_cast<Asset<T>>(CacheAsset(asset_path, std::move(Asset<T> { asset_path, std::move(result.value()) })));
                        } catch (const std::bad_any_cast& e) {
                            return Failure(std::format("Failed to load asset \"{}\": {}", asset_path, e.what()));
                        }
                    } else {
                        return Failure(result.error());
                    }
                } else {
                    delete archive;
                }
            }
            return Failure(std::format("Failed to load asset \"{}\": Asset cannot be found!", asset_path));
        }

        FOW_SHARED_API Result<String> LoadFromArchiveAsString(const Path& archive_path, const Path& asset_path);
        FOW_SHARED_API Result<Vector<uint8_t>> LoadFromArchiveAsBytes(const Path& archive_path, const Path& asset_path);
        FOW_SHARED_API Result<pugi::xml_document> LoadFromArchiveAsXml(const Path& archive_path, const Path& asset_path);
        FOW_SHARED_API Result<nlohmann::json> LoadFromArchiveAsJson(const Path& archive_path, const Path& asset_path);
        FOW_SHARED_API Result<String> LoadAsString(const Path& asset_path, AssetLoaderFlags::Type flags = AssetLoaderFlags::Default);
        FOW_SHARED_API Result<Vector<uint8_t>> LoadAsBytes(const Path& asset_path, AssetLoaderFlags::Type flags = AssetLoaderFlags::Default);
        FOW_SHARED_API Result<pugi::xml_document> LoadAsXml(const Path& asset_path, AssetLoaderFlags::Type flags = AssetLoaderFlags::Default);
        FOW_SHARED_API Result<nlohmann::json> LoadAsJson(const Path& asset_path, AssetLoaderFlags::Type flags = AssetLoaderFlags::Default);

        FOW_SHARED_API Result<HashMap<String, String>> LoadStringsFromAllArchives(const Path& asset_path, AssetLoaderFlags::Type flags = AssetLoaderFlags::Default);
    }
}

#endif