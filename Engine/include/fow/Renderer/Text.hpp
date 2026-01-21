#ifndef FOW_RENDERER_TEXT_HPP
#define FOW_RENDERER_TEXT_HPP

#include "fow/Shared.hpp"
#include "fow/String.hpp"
#include "fow/Result.hpp"

#include <ft2build.h>
#include FT_FREETYPE_H

namespace fow {
    class Font;
    using FontPtr = SharedPtr<Font>;

    class Text;

    class FOW_RENDER_API Font {
        FT_Face m_pFace;

        explicit Font(const FT_Face& face) : m_pFace(face) { }
    public:
        Font(const Font& other) = default;
        Font(Font&& other) noexcept : m_pFace(other.m_pFace) { other.m_pFace = nullptr; }
        ~Font();

        static Result<FontPtr> LoadAsset(const Path& path, AssetLoaderFlags::Type flags);
    };
}

#endif