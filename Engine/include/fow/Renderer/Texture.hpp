#ifndef FOW_RENDERER_TEXTURE_HPP
#define FOW_RENDERER_TEXTURE_HPP

#include <glad/glad.h>
#include <pugixml.hpp>
#include <rfl.hpp>
#include <rfl/xml.hpp>

#include "fow/Shared.hpp"

#define FOW_TEXTURE_PLACEHOLDER_ASSET_PATH "FOGOFWAR::Textures/Null"
#ifndef FOW_TEXTURE_PLACEHOLDER_SIZE
    #define FOW_TEXTURE_PLACEHOLDER_SIZE 16
#endif

namespace fow {
    class Shader;

    class Texture;
    class Texture2D;
    class Texture2DArray;
    class TextureCubeMap;

    enum class TextureTarget : GLenum {
        Texture1D           = GL_TEXTURE_1D,
        Texture1DArray      = GL_TEXTURE_1D_ARRAY,
        Texture2D           = GL_TEXTURE_2D,
        Texture2DArray      = GL_TEXTURE_2D_ARRAY,
        Texture3D           = GL_TEXTURE_3D,
        TextureCubeMap      = GL_TEXTURE_CUBE_MAP,
        TextureCubeMapArray = GL_TEXTURE_CUBE_MAP_ARRAY
    };
    enum class TextureWrapMode : GLenum {
        Repeat                = GL_REPEAT,
        MirroredRepeat        = GL_MIRRORED_REPEAT,
        ClampToEdge           = GL_CLAMP_TO_EDGE,
        ClampToBorder         = GL_CLAMP_TO_BORDER,
        MirroredClampToEdge   = GL_MIRROR_CLAMP_TO_EDGE,
        MirroredClampToBorder = GL_MIRROR_CLAMP_TO_BORDER_EXT
    };
    enum class TexturePixelFormat : GLenum {
        Luminance      = GL_RED,
        LuminanceAlpha = GL_RG,
        RGB            = GL_RGB,
        RGBA           = GL_RGBA
    };
    enum class TextureInternalPixelFormat : GLenum {
        Luminance           = GL_LUMINANCE8_EXT,
        LuminanceAlpha      = GL_LUMINANCE8_ALPHA8_EXT,
        RGB                 = GL_RGB8,
        RGBA                = GL_RGBA8,
        CompressedRGTC1     = GL_COMPRESSED_RED_RGTC1,
        CompressedRGTC1S    = GL_COMPRESSED_SIGNED_RED_RGTC1,
        CompressedRGTC2     = GL_COMPRESSED_RED_GREEN_RGTC2_EXT,
        CompressedDxt1      = GL_COMPRESSED_RGB_S3TC_DXT1_EXT,
        CompressedDxt1Alpha = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT,
        CompressedDxt3      = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT,
        CompressedDxt5      = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT,
        CompressedBC6S      = GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT,
        CompressedBC6U      = GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT,
        CompressedBC7U      = GL_COMPRESSED_RGBA_BPTC_UNORM
    };
    enum class TextureMagFilterMode : GLenum {
        Nearest = GL_NEAREST,
        Linear  = GL_LINEAR
    };
    enum class TextureMinFilterMode : GLenum {
        Nearest              = GL_NEAREST,
        Linear               = GL_LINEAR,
        NearestMipmapNearest = GL_NEAREST_MIPMAP_NEAREST,
        NearestMipmapLinear  = GL_NEAREST_MIPMAP_LINEAR,
        LinearMipmapNearest  = GL_LINEAR_MIPMAP_NEAREST,
        LinearMipmapLinear   = GL_LINEAR_MIPMAP_LINEAR
    };

    struct FOW_RENDER_API TextureInfo {
        std::string Source;
        Option<int> FrameCount;
        Option<TextureTarget> Target;
        Option<TextureMagFilterMode> MagFilter;
        Option<TextureMinFilterMode> MinFilter;
        Option<TextureWrapMode> WrapS;
        Option<TextureWrapMode> WrapT;
        Option<bool> GenerateMipMaps;
    };

    template<typename T>
    concept TextureType = std::is_assignable_v<Texture, T>;

    using TexturePtr = SharedPtr<Texture>;
    using Texture2DPtr = SharedPtr<Texture2D>;
    using Texture2DArrayPtr = SharedPtr<Texture2DArray>;
    using TextureCubeMapPtr = SharedPtr<TextureCubeMap>;

    class FOW_RENDER_API Texture {
    protected:
        GLuint m_uId;
        bool   m_bInitialized;

        Texture(const GLuint id) : m_uId(id), m_bInitialized(true) { }
    public:
        Texture() : m_uId(0), m_bInitialized(false) { }
        Texture(const Texture& other) : m_uId(other.m_uId), m_bInitialized(other.m_bInitialized) { }
        Texture(Texture&& other) noexcept : m_uId(other.m_uId), m_bInitialized(other.m_bInitialized) {
            other.m_uId = 0;
            other.m_bInitialized = false;
        }
        virtual ~Texture();

        Texture& operator= (const Texture& other) {
            if (m_uId != 0 && m_bInitialized) {
                glDeleteTextures(1, &m_uId);
            }
            m_uId = other.m_uId;
            m_bInitialized = other.m_bInitialized;
            return *this;
        }
        Texture& operator= (Texture&& other) noexcept {
            if (m_uId != 0 && m_bInitialized) {
                glDeleteTextures(1, &m_uId);
            }
            m_uId = other.m_uId;
            m_bInitialized = other.m_bInitialized;
            other.m_uId = 0;
            other.m_bInitialized = false;
            return *this;
        }

        [[nodiscard]] constexpr GLuint id() const { return m_uId; }
        [[nodiscard]] constexpr bool is_valid() const { return m_uId != 0; }
        ABSTRACT(TextureTarget target() const);

        [[nodiscard]] GLsizei width() const;
        [[nodiscard]] GLsizei height() const;
        [[nodiscard]] GLsizei depth() const;
        [[nodiscard]] GLsizei base_level() const;
        [[nodiscard]] GLsizei max_level() const;

        [[nodiscard]] TextureWrapMode wrap_r() const;
        [[nodiscard]] TextureWrapMode wrap_s() const;
        [[nodiscard]] TextureWrapMode wrap_t() const;
        void wrap_r(TextureWrapMode mode) const;
        void wrap_s(TextureWrapMode mode) const;
        void wrap_t(TextureWrapMode mode) const;

        [[nodiscard]] TextureMagFilterMode mag_filter() const;
        [[nodiscard]] TextureMinFilterMode min_filter() const;
        void mag_filter(TextureMagFilterMode mode) const;
        void min_filter(TextureMinFilterMode mode) const;

        void generate_mipmaps() const;

        virtual void bind(const uint8_t unit) {
            glActiveTexture(GL_TEXTURE0 + (unit % 32));
            glBindTexture(static_cast<GLenum>(target()), m_uId);
        }

        static TexturePtr PlaceHolder();
        static TexturePtr DefaultWhite();
        static TexturePtr DefaultBlack();
        static TexturePtr DefaultNormal();
        static void UnloadPlaceHolder();
    };

    class FOW_RENDER_API Texture2D final : public Texture {
    protected:
        explicit Texture2D(const GLuint id) : Texture(id) { }
    public:
        Texture2D() = default;
        Texture2D(const Texture2D& other) = default;
        Texture2D(Texture2D&& other) noexcept : Texture(std::move(other)) { }

        Texture2D& operator= (const Texture2D& other) = default;
        Texture2D& operator= (Texture2D&& other) noexcept = default;

        [[nodiscard]] constexpr TextureTarget target() const override { return TextureTarget::Texture2D; }

        static Result<Texture2DPtr> Load(const TextureInfo& info);
        static Result<Texture2DPtr> LoadFromMemory(const Vector<uint8_t>& data, const TextureInfo& info);
        static Result<Texture2DPtr> LoadAsset(const Path& path, AssetLoaderFlags::Type flags);

        friend class Texture;
        friend class Shader;
    };
    class FOW_RENDER_API Texture2DArray final : public Texture {
    protected:
        explicit Texture2DArray(const GLuint id) : Texture(id) { }
    public:
        Texture2DArray() = default;
        Texture2DArray(const Texture2DArray& other) = default;
        Texture2DArray(Texture2DArray&& other) noexcept : Texture(std::move(other)) { }

        Texture2DArray& operator= (const Texture2DArray& other) = default;
        Texture2DArray& operator= (Texture2DArray&& other) noexcept = default;

        [[nodiscard]] constexpr TextureTarget target() const override { return TextureTarget::Texture2DArray; }

        static Result<Texture2DArrayPtr> Load(const TextureInfo& info);
        static Result<Texture2DArrayPtr> LoadFromMemory(const Vector<uint8_t>& data, const TextureInfo& info);
        static Result<Texture2DArrayPtr> LoadAsset(const Path& path, AssetLoaderFlags::Type flags);

        friend class Texture;
        friend class Shader;
    };
    class FOW_RENDER_API TextureCubeMap final : public Texture {
    protected:
        explicit TextureCubeMap(const GLuint id) : Texture(id) { }
    public:
        TextureCubeMap() = default;
        TextureCubeMap(const TextureCubeMap& other) = default;
        TextureCubeMap(TextureCubeMap&& other) noexcept : Texture(std::move(other)) { }

        TextureCubeMap& operator= (const TextureCubeMap& other) = default;
        TextureCubeMap& operator= (TextureCubeMap&& other) noexcept = default;

        [[nodiscard]] constexpr TextureTarget target() const override { return TextureTarget::TextureCubeMap; }

        static Result<TextureCubeMapPtr> Load(const TextureInfo& info);
        static Result<TextureCubeMapPtr> LoadFromMemory(const Vector<uint8_t>& data, const TextureInfo& info);
        static Result<TextureCubeMapPtr> LoadAsset(const Path& path, AssetLoaderFlags::Type flags);

        friend class Texture;
        friend class Shader;
    };
}

#endif