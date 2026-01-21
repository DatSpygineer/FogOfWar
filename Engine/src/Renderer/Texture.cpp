#include "fow/Renderer/Texture.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "fow/Renderer/ThirdParty/stb_image.h"

namespace fow {
    Texture::~Texture() {
        if (m_uId != 0 && m_bInitialized) {
            glDeleteTextures(1, &m_uId);
        }
    }

    GLsizei Texture::width() const {
        GLsizei value;
        glGetTextureParameteriv(m_uId, GL_TEXTURE_WIDTH, &value);
        return value;
    }
    GLsizei Texture::height() const {
        GLsizei value;
        glGetTextureParameteriv(m_uId, GL_TEXTURE_HEIGHT, &value);
        return value;
    }
    GLsizei Texture::depth() const {
        GLsizei value;
        glGetTextureParameteriv(m_uId, GL_TEXTURE_DEPTH, &value);
        return value;
    }
    GLsizei Texture::base_level() const {
        GLsizei value;
        glGetTextureParameteriv(m_uId, GL_TEXTURE_BASE_LEVEL, &value);
        return value;
    }
    GLsizei Texture::max_level() const {
        GLsizei value;
        glGetTextureParameteriv(m_uId, GL_TEXTURE_MAX_LEVEL, &value);
        return value;
    }

    TextureWrapMode Texture::wrap_r() const {
        GLint value;
        glGetTextureParameteriv(m_uId, GL_TEXTURE_WRAP_R, &value);
        return static_cast<TextureWrapMode>(value);
    }
    TextureWrapMode Texture::wrap_s() const {
        GLint value;
        glGetTextureParameteriv(m_uId, GL_TEXTURE_WRAP_S, &value);
        return static_cast<TextureWrapMode>(value);
    }
    TextureWrapMode Texture::wrap_t() const {
        GLint value;
        glGetTextureParameteriv(m_uId, GL_TEXTURE_WRAP_T, &value);
        return static_cast<TextureWrapMode>(value);
    }
    void Texture::wrap_r(TextureWrapMode mode) const {
        glTextureParameteri(m_uId, GL_TEXTURE_WRAP_R, static_cast<GLint>(mode));
    }
    void Texture::wrap_s(TextureWrapMode mode) const {
        glTextureParameteri(m_uId, GL_TEXTURE_WRAP_S, static_cast<GLint>(mode));
    }
    void Texture::wrap_t(TextureWrapMode mode) const {
        glTextureParameteri(m_uId, GL_TEXTURE_WRAP_T, static_cast<GLint>(mode));
    }
    TextureMagFilterMode Texture::mag_filter() const {
        GLint value;
        glGetTextureParameteriv(m_uId, GL_TEXTURE_MAG_FILTER, &value);
        return static_cast<TextureMagFilterMode>(value);
    }
    TextureMinFilterMode Texture::min_filter() const {
        GLint value;
        glGetTextureParameteriv(m_uId, GL_TEXTURE_MIN_FILTER, &value);
        return static_cast<TextureMinFilterMode>(value);
    }
    void Texture::mag_filter(TextureMagFilterMode mode) const {
        glTextureParameteri(m_uId, GL_TEXTURE_MAG_FILTER, static_cast<GLint>(mode));
    }
    void Texture::min_filter(TextureMinFilterMode mode) const {
        glTextureParameteri(m_uId, GL_TEXTURE_MIN_FILTER, static_cast<GLint>(mode));
    }
    void Texture::generate_mipmaps() const {
        glGenerateTextureMipmap(m_uId);
    }

    static TexturePtr s_placeholder_texture = nullptr;

    TexturePtr Texture::PlaceHolder() {
        if (s_placeholder_texture != nullptr) {
            return s_placeholder_texture;
        }
        Vector<uint32_t> data;
        data.reserve(FOW_TEXTURE_PLACEHOLDER_SIZE * FOW_TEXTURE_PLACEHOLDER_SIZE);
        for (int x = 0; x < FOW_TEXTURE_PLACEHOLDER_SIZE; ++x) {
            for (int y = 0; y < FOW_TEXTURE_PLACEHOLDER_SIZE; ++y) {
                data.emplace_back(
                    x % 4 >= 2 != y % 4 >= 2 ?
                    0xFFFF00FF :
                    0xFF000000
                );
            }
        }

        GLuint id;
        glGenTextures(1, &id);
        if (id == 0) {
            throw std::runtime_error(std::format("Failed to generate placeholder texture: GL error {}", glGetError()));
        }

        glBindTexture(GL_TEXTURE_2D, id);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, FOW_TEXTURE_PLACEHOLDER_SIZE, FOW_TEXTURE_PLACEHOLDER_SIZE, 0, GL_RGBA, GL_UNSIGNED_BYTE, data.data());
        glTextureParameteri(id, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTextureParameteri(id, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTextureParameteri(id, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTextureParameteri(id, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glBindTexture(GL_TEXTURE_2D, 0);
        s_placeholder_texture = std::make_shared<Texture2D>(std::move(Texture2D { id }));
        return s_placeholder_texture;
    }

    void Texture::UnloadPlaceHolder() {
        if (s_placeholder_texture != nullptr) {
            s_placeholder_texture = nullptr;
        }
    }

    static Result<GLuint> LoadOpenGLTexture(const Vector<uint8_t>& data, const TextureInfo& info) {
        Result<GLuint> result;
        GLuint id = 0;
        GLint format = GL_RGBA;
        GLint iformat = GL_RGBA;

        const auto mag_filter = info.MagFilter.value_or(TextureMagFilterMode::Linear);
        const auto min_filter = info.MinFilter.value_or(TextureMinFilterMode::Linear);
        const auto wrap_s = info.WrapS.value_or(TextureWrapMode::Repeat);
        const auto wrap_t = info.WrapT.value_or(TextureWrapMode::Repeat);
        const auto frame_count = info.FrameCount.value_or(1);
        const auto target = info.Target.value_or(TextureTarget::Texture2D);
        const auto gl_target = static_cast<GLenum>(target);
        int w, h, c;

        const auto image_data = stbi_load_from_memory(data.data(), static_cast<int>(data.size()), &w, &h, &c, 0);
        if (image_data == nullptr) {
            result = Failure<GLuint>(std::format("Failed to read image data from asset \"{}\"", info.Source));
            goto LOAD_GL_TEXTURE_END;
        }

        glCreateTextures(gl_target, 1, &id);

        if (id == 0) {
            result = Failure<GLuint>(std::format("Failed to generate OpenGL texture handle: GL error \"{}\"", glGetError()));
            goto LOAD_GL_TEXTURE_END;
        }

        switch (c) {
            case 1: format = GL_RED; iformat = GL_LUMINANCE8_EXT; break;
            case 2: format = GL_RG;  iformat = GL_LUMINANCE8_ALPHA8_EXT; break;
            case 3: format = iformat = GL_RGB; break;
            default: format = iformat = GL_RGBA; break;
        }

        glBindTexture(gl_target, id);
        switch (target) {
            case TextureTarget::Texture1D: {
                glTexImage1D(gl_target, 0, iformat, w * h, 0, format, GL_UNSIGNED_BYTE, image_data);
            } break;
            case TextureTarget::Texture1DArray: {
                glTexImage2D(gl_target, 0, iformat, w, frame_count, 0, format, GL_UNSIGNED_BYTE, image_data);
            } break;
            case TextureTarget::Texture2D: {
                glTexImage2D(gl_target, 0, iformat, w, h, 0, format, GL_UNSIGNED_BYTE, image_data);
            } break;
            case TextureTarget::Texture2DArray: {
                glTexImage3D(gl_target, 0, iformat, w, h, frame_count, 0, format, GL_UNSIGNED_BYTE, image_data);
            } break;
            case TextureTarget::Texture3D: {
                result = Failure<GLuint>(std::format("Target \"Texture3D\" is not supported!"));
                goto LOAD_GL_TEXTURE_END;
            }
            case TextureTarget::TextureCubeMap: {
                if (h != w * 6) {
                    result = Failure<GLuint>(std::format("Invalid cubemap size: {}x{}, expected: {}x{}", w, h, w, w * 6));
                    goto LOAD_GL_TEXTURE_END;
                }

                size_t offset = 0;
                for (int i = 0; i < 6; ++i) {
                    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, iformat, w / 6, h, 0, format, GL_UNSIGNED_BYTE, image_data + offset);
                    offset += c * w * w;
                }
            } break;
            case TextureTarget::TextureCubeMapArray: {
                result = Failure<GLuint>(std::format("Target \"TextureCubeMapArray\" is not supported!"));
                goto LOAD_GL_TEXTURE_END;
            }
        }

        glTextureParameteri(id, GL_TEXTURE_MAG_FILTER, static_cast<GLint>(mag_filter));
        glTextureParameteri(id, GL_TEXTURE_MIN_FILTER, static_cast<GLint>(min_filter));
        glTextureParameteri(id, GL_TEXTURE_WRAP_S,     static_cast<GLint>(wrap_s)    );
        glTextureParameteri(id, GL_TEXTURE_WRAP_T,     static_cast<GLint>(wrap_t)    );

        if (info.GenerateMipMaps) {
            glGenerateTextureMipmap(id);
        }

        glBindTexture(gl_target, 0);
        result = Success<GLuint>(id);

    LOAD_GL_TEXTURE_END:
        if (!result.has_value() && id != 0) {
            glDeleteTextures(1, &id);
        }
        if (image_data != nullptr) {
            stbi_image_free(image_data);
        }
        return result;
    }

    Result<Texture2DPtr> Texture2D::Load(const TextureInfo& info) {
        const auto image_data = Assets::LoadAsBytes(info.Source.c_str());
        if (!image_data.has_value()) {
            return Failure<Texture2DPtr>(std::format("Failed to load image \"{}\": Image asset doesn't exists!", info.Source));
        }
        return LoadFromMemory(image_data.value(), info);
    }
    Result<Texture2DPtr> Texture2D::LoadFromMemory(const Vector<uint8_t>& data, const TextureInfo& info) {
        auto id_result = LoadOpenGLTexture(data, info);
        if (id_result.has_value()) {
            return Success<Texture2DPtr>(std::make_shared<Texture2D>(std::move(Texture2D { id_result.value() })));
        }
        return Failure<Texture2DPtr>(std::format("Failed to load image data \"{}\": {}", info.Source, id_result.error().message), id_result.error().location);
    }

    Result<Texture2DPtr> Texture2D::LoadAsset(const Path& path, const AssetLoaderFlags::Type flags) {
        if (!path.extension().equals(".xml", StringCompareType::CaseInsensitive)) {
            return Failure<Texture2DPtr>(std::format("Failed to load texture \"{}\": Expected asset extension '.xml'", path));
        }
        const auto xml_str = Assets::LoadAsString(path, flags);
        if (!xml_str.has_value()) {
            return Failure<Texture2DPtr>(std::format("Failed to load texture \"{}\": {}", path, xml_str.error().message));
        }
        const auto info_value = rfl::xml::read<TextureInfo>(xml_str.value().as_std_str());
        if (!info_value.has_value()) {
            return Failure<Texture2DPtr>(std::format("Failed to load texture \"{}\": {}", path, info_value.error().what()));
        }
        if (info_value->Target.value_or(TextureTarget::Texture2D) != TextureTarget::Texture2D) {
            return Failure<Texture2DPtr>(std::format("Failed to load texture \"{}\": Expected target 'Texture2D'", path));
        }
        if (const auto image_data = Assets::LoadAsBytes(info_value->Source.c_str(), flags); image_data.has_value()) {
            return LoadFromMemory(image_data.value(), info_value.value());
        }
        return Failure<Texture2DPtr>(std::format("Failed to load texture \"{}\": Image data \"{}\" cannot be found!", path, info_value->Source));
    }

    Result<Texture2DArrayPtr> Texture2DArray::Load(const TextureInfo& info) {
        const auto image_data = Assets::LoadAsBytes(info.Source.c_str());
        if (!image_data.has_value()) {
            return Failure<Texture2DArrayPtr>(std::format("Failed to load image \"{}\": Image asset doesn't exists!", info.Source));
        }
        return LoadFromMemory(image_data.value(), info);
    }
    Result<Texture2DArrayPtr> Texture2DArray::LoadFromMemory(const Vector<uint8_t>& data, const TextureInfo& info) {
        auto id_result = LoadOpenGLTexture(data, info);
        if (id_result.has_value()) {
            return Success<Texture2DArrayPtr>(std::make_shared<Texture2DArray>(std::move(Texture2DArray { id_result.value() })));
        }
        return Failure<Texture2DArrayPtr>(std::format("Failed to load image data \"{}\": {}", info.Source, id_result.error().message), id_result.error().location);
    }

    Result<Texture2DArrayPtr> Texture2DArray::LoadAsset(const Path& path, const AssetLoaderFlags::Type flags) {
        if (!path.extension().equals(".xml", StringCompareType::CaseInsensitive)) {
            return Failure<Texture2DArrayPtr>(std::format("Failed to load texture \"{}\": Expected asset extension '.xml'", path));
        }
        const auto xml_str = Assets::LoadAsString(path, flags);
        if (!xml_str.has_value()) {
            return Failure<Texture2DArrayPtr>(std::format("Failed to load texture \"{}\": {}", path, xml_str.error().message));
        }
        const auto info_value = rfl::xml::read<TextureInfo>(xml_str.value().as_std_str());
        if (!info_value.has_value()) {
            return Failure<Texture2DArrayPtr>(std::format("Failed to load texture \"{}\": {}", path, info_value.error().what()));
        }
        if (info_value->Target.value_or(TextureTarget::Texture2DArray) != TextureTarget::Texture2DArray) {
            return Failure<Texture2DArrayPtr>(std::format("Failed to load texture \"{}\": Expected target 'Texture2DArray'", path));
        }
        if (const auto image_data = Assets::LoadAsBytes(info_value->Source.c_str(), flags); image_data.has_value()) {
            return LoadFromMemory(image_data.value(), info_value.value());
        }
        return Failure<Texture2DArrayPtr>(std::format("Failed to load texture \"{}\": Image data \"{}\" cannot be found!", path, info_value->Source));
    }

    Result<TextureCubeMapPtr> TextureCubeMap::Load(const TextureInfo& info) {
        const auto image_data = Assets::LoadAsBytes(info.Source.c_str());
        if (!image_data.has_value()) {
            return Failure<TextureCubeMapPtr>(std::format("Failed to load image \"{}\": Image asset doesn't exists!", info.Source));
        }
        return LoadFromMemory(image_data.value(), info);
    }
    Result<TextureCubeMapPtr> TextureCubeMap::LoadFromMemory(const Vector<uint8_t>& data, const TextureInfo& info) {
        auto id_result = LoadOpenGLTexture(data, info);
        if (id_result.has_value()) {
            return Success<TextureCubeMapPtr>(std::make_shared<TextureCubeMap>(std::move(TextureCubeMap { id_result.value() })));
        }
        return Failure<TextureCubeMapPtr>(std::format("Failed to load image data \"{}\": {}", info.Source, id_result.error().message), id_result.error().location);
    }

    Result<TextureCubeMapPtr> TextureCubeMap::LoadAsset(const Path& path, const AssetLoaderFlags::Type flags) {
        if (!path.extension().equals(".xml", StringCompareType::CaseInsensitive)) {
            return Failure<TextureCubeMapPtr>(std::format("Failed to load texture \"{}\": Expected asset extension '.xml'", path));
        }
        const auto xml_str = Assets::LoadAsString(path, flags);
        if (!xml_str.has_value()) {
            return Failure<TextureCubeMapPtr>(std::format("Failed to load texture \"{}\": {}", path, xml_str.error().message));
        }
        const auto info_value = rfl::xml::read<TextureInfo>(xml_str.value().as_std_str());
        if (!info_value.has_value()) {
            return Failure<TextureCubeMapPtr>(std::format("Failed to load texture \"{}\": {}", path, info_value.error().what()));
        }
        if (info_value->Target.value_or(TextureTarget::TextureCubeMap) != TextureTarget::TextureCubeMap) {
            return Failure<TextureCubeMapPtr>(std::format("Failed to load texture \"{}\": Expected target 'TextureCubeMap'", path));
        }
        if (const auto image_data = Assets::LoadAsBytes(info_value->Source.c_str(), flags); image_data.has_value()) {
            return LoadFromMemory(image_data.value(), info_value.value());
        }
        return Failure<TextureCubeMapPtr>(std::format("Failed to load texture \"{}\": Image data \"{}\" cannot be found!", path, info_value->Source));
    }
}
