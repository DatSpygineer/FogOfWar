#include "fow/Renderer.hpp"

namespace fow {
    Font::~Font() {
        FT_Done_Face(m_pFace);
    }

    Result<FontPtr> Font::LoadAsset(const Path& path, const AssetLoaderFlags::Type flags) {
        const auto data = Assets::LoadAsBytes(path, flags);
        if (!data.has_value()) {
            return Failure<FontPtr>(std::format("Failed to load font \"{}\": Could not ready assets file!", path));
        }

        FT_Face face = nullptr;
        if (const auto error = FT_New_Memory_Face(Renderer::GetFreetypeLibrary(), data->data(), data->size(), 0, &face); error != FT_Err_Ok) {
            return Failure<FontPtr>(std::format("Failed to load font \"{}\": {}", path, FT_Error_String(error)));
        }
        return Success<FontPtr>(std::move(std::make_shared<Font>(std::move(Font { face }))));
    }
}
