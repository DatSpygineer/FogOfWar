#include <glad/glad.h>
#include "fow/Renderer/Texture.hpp"
#include "fow/Shared/StringConvertion.hpp"

#include "SOIL2.h"

namespace fow {
    template<>
    Result<TextureTarget> StringToEnum(const String& value) {
        if (value.equals("Texture1D", StringCompareType::CaseInsensitive)) {
            return TextureTarget::Texture1D;
        }
        if (value.equals("Texture1DArray", StringCompareType::CaseInsensitive)) {
            return TextureTarget::Texture1DArray;
        }
        if (value.equals("Texture2D", StringCompareType::CaseInsensitive)) {
            return TextureTarget::Texture2D;
        }
        if (value.equals("Texture2DArray", StringCompareType::CaseInsensitive)) {
            return TextureTarget::Texture2DArray;
        }
        if (value.equals("Texture3D", StringCompareType::CaseInsensitive)) {
            return TextureTarget::Texture3D;
        }
        if (value.equals("TextureCubeMap", StringCompareType::CaseInsensitive)) {
            return TextureTarget::TextureCubeMap;
        }
        if (value.equals("TextureCubeMapArray", StringCompareType::CaseInsensitive)) {
            return TextureTarget::TextureCubeMapArray;
        }

        if (const auto result = rfl::string_to_enum<TextureTarget>(value.as_std_str()); result.has_value()) {
            return result.value();
        } else {
            return Failure(result.error().what());
        }
    }
    template<>
    Result<TextureWrapMode> StringToEnum(const String& value) {
        if (value.equals("Repeat", StringCompareType::CaseInsensitive)) {
            return TextureWrapMode::Repeat;
        }
        if (value.equals("MirroredRepeat", StringCompareType::CaseInsensitive)) {
            return TextureWrapMode::MirroredRepeat;
        }
        if (value.equals("ClampToEdge", StringCompareType::CaseInsensitive)) {
            return TextureWrapMode::ClampToEdge;
        }
        if (value.equals("ClampToBorder", StringCompareType::CaseInsensitive)) {
            return TextureWrapMode::ClampToBorder;
        }
        if (value.equals("MirroredClampToEdge", StringCompareType::CaseInsensitive)) {
            return TextureWrapMode::MirroredClampToEdge;
        }
        if (value.equals("MirroredClampToBorder", StringCompareType::CaseInsensitive)) {
            return TextureWrapMode::MirroredClampToBorder;
        }
        if (const auto result = rfl::string_to_enum<TextureWrapMode>(value.as_std_str()); result.has_value()) {
            return result.value();
        } else {
            return Failure(result.error().what());
        }
    }
    template<>
    Result<TextureMagFilterMode> StringToEnum(const String& value) {
        if (value.equals("Nearest", StringCompareType::CaseInsensitive)) {
            return TextureMagFilterMode::Nearest;
        }
        if (value.equals("Linear", StringCompareType::CaseInsensitive)) {
            return TextureMagFilterMode::Linear;
        }
        if (const auto result = rfl::string_to_enum<TextureMagFilterMode>(value.as_std_str()); result.has_value()) {
            return result.value();
        } else {
            return Failure(result.error().what());
        }
    }
    template<>
    Result<TextureMinFilterMode> StringToEnum(const String& value) {
        if (value.equals("Nearest", StringCompareType::CaseInsensitive)) {
            return TextureMinFilterMode::Nearest;
        }
        if (value.equals("Linear", StringCompareType::CaseInsensitive)) {
            return TextureMinFilterMode::Linear;
        }
        if (value.equals("NearestMipmapNearest", StringCompareType::CaseInsensitive)) {
            return TextureMinFilterMode::NearestMipmapNearest;
        }
        if (value.equals("NearestMipmapLinear", StringCompareType::CaseInsensitive)) {
            return TextureMinFilterMode::NearestMipmapLinear;
        }
        if (value.equals("LinearMipmapNearest", StringCompareType::CaseInsensitive)) {
            return TextureMinFilterMode::LinearMipmapNearest;
        }
        if (value.equals("LinearMipmapLinear", StringCompareType::CaseInsensitive)) {
            return TextureMinFilterMode::LinearMipmapLinear;
        }
        if (const auto result = rfl::string_to_enum<TextureMinFilterMode>(value.as_std_str()); result.has_value()) {
            return result.value();
        } else {
            return Failure(result.error().what());
        }
    }

    static Result<TextureInfo> ParseTextureInfo(const String& src) {
        pugi::xml_document doc;
        if (const auto result = doc.load_string(src.as_cstr()); result.status != pugi::status_ok) {
            return Failure(result.description());
        }

        const auto root = doc.child("Texture");
        if (!root) {
            return Failure("Expected root node \"Texture\"");
        }

        const auto src_attrib = root.attribute("src");
        if (!src_attrib) {
            return Failure("Expected attribute \"src\" on root node");
        }

        TextureInfo info;
        info.Source = src_attrib.value();

        for (const auto& node : root.children()) {
            if (String(node.name()) == "Target") {
                if (const auto result = StringToEnum<TextureTarget>(node.child_value()); result.has_value()) {
                    info.Target = result.value();
                } else {
                    return Failure(std::format("Failed to parse node \"Target\" with value \"{}\": {}", node.child_value(), result.error().message));
                }
            } else if (String(node.name()) == "MagFilter") {
                if (const auto result = StringToEnum<TextureMagFilterMode>(node.child_value()); result.has_value()) {
                    info.MagFilter = result.value();
                } else {
                    return Failure(std::format("Failed to parse node \"MagFilter\" with value \"{}\": {}", node.child_value(), result.error().message));
                }
            } else if (String(node.name()) == "MinFilter") {
                if (const auto result = StringToEnum<TextureMinFilterMode>(node.child_value()); result.has_value()) {
                    info.MinFilter = result.value();
                } else {
                    return Failure(std::format("Failed to parse node \"MinFilter\" with value \"{}\": {}", node.child_value(), result.error().message));
                }
            } else if (String(node.name()) == "WrapS") {
                if (const auto result = StringToEnum<TextureWrapMode>(node.child_value()); result.has_value()) {
                    info.WrapS = result.value();
                } else {
                    return Failure(std::format("Failed to parse node \"WrapS\" with value \"{}\": {}", node.child_value(), result.error().message));
                }
            } else if (String(node.name()) == "WrapT") {
                if (const auto result = StringToEnum<TextureWrapMode>(node.child_value()); result.has_value()) {
                    info.WrapT = result.value();
                } else {
                    return Failure(std::format("Failed to parse node \"WrapT\" with value \"{}\": {}", node.child_value(), result.error().message));
                }
            } else if (String(node.name()) == "FrameCount") {
                try {
                    info.FrameCount = std::stoi(node.child_value());
                } catch (const std::exception& e) {
                    return Failure("Expected integer value for node \"FrameCount\"");
                }
            } else if (String(node.name()) == "GenerateMipMaps") {
                if (const auto result = StringToBool(node.child_value()); result.has_value()) {
                    info.GenerateMipMaps = result.value();
                } else {
                    return Failure(std::format("Expected boolean value for \"GenerateMipMaps\""));
                }
            } else {
                Debug::LogError(std::format("Unknown parameter \"{}\"", node.name()));
            }
        }
        return info;
    }

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
    static TexturePtr s_white_texture = nullptr;
    static TexturePtr s_black_texture = nullptr;
    static TexturePtr s_normal_texture = nullptr;

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
        glTextureParameteri(id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTextureParameteri(id, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(id, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTextureParameteri(id, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glBindTexture(GL_TEXTURE_2D, 0);
        s_placeholder_texture = std::make_shared<Texture2D>(std::move(Texture2D { id }));
        return s_placeholder_texture;
    }
    TexturePtr Texture::DefaultWhite() {
        if (s_white_texture != nullptr) {
            return s_white_texture;
        }
        Vector<uint32_t> data;
        data.reserve(FOW_TEXTURE_PLACEHOLDER_SIZE * FOW_TEXTURE_PLACEHOLDER_SIZE);
        for (int i = 0; i < FOW_TEXTURE_PLACEHOLDER_SIZE * FOW_TEXTURE_PLACEHOLDER_SIZE; ++i) {
            data.emplace_back(0xFFFFFFFF);
        }

        GLuint id;
        glGenTextures(1, &id);
        if (id == 0) {
            throw std::runtime_error(std::format("Failed to generate default white texture: GL error {}", glGetError()));
        }

        glBindTexture(GL_TEXTURE_2D, id);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, FOW_TEXTURE_PLACEHOLDER_SIZE, FOW_TEXTURE_PLACEHOLDER_SIZE, 0, GL_RGBA, GL_UNSIGNED_BYTE, data.data());
        glTextureParameteri(id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTextureParameteri(id, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(id, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTextureParameteri(id, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glBindTexture(GL_TEXTURE_2D, 0);
        s_white_texture = std::make_shared<Texture2D>(std::move(Texture2D { id }));
        return s_white_texture;
    }
    TexturePtr Texture::DefaultBlack() {
        if (s_black_texture != nullptr) {
            return s_black_texture;
        }
        Vector<uint32_t> data;
        data.reserve(FOW_TEXTURE_PLACEHOLDER_SIZE * FOW_TEXTURE_PLACEHOLDER_SIZE);
        for (int i = 0; i < FOW_TEXTURE_PLACEHOLDER_SIZE * FOW_TEXTURE_PLACEHOLDER_SIZE; ++i) {
            data.emplace_back(0xFF000000);
        }

        GLuint id;
        glGenTextures(1, &id);
        if (id == 0) {
            throw std::runtime_error(std::format("Failed to generate default black texture: GL error {}", glGetError()));
        }

        glBindTexture(GL_TEXTURE_2D, id);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, FOW_TEXTURE_PLACEHOLDER_SIZE, FOW_TEXTURE_PLACEHOLDER_SIZE, 0, GL_RGBA, GL_UNSIGNED_BYTE, data.data());
        glTextureParameteri(id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTextureParameteri(id, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(id, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTextureParameteri(id, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glBindTexture(GL_TEXTURE_2D, 0);
        s_black_texture = std::make_shared<Texture2D>(std::move(Texture2D { id }));
        return s_black_texture;
    }
    TexturePtr Texture::DefaultNormal() {
        if (s_normal_texture != nullptr) {
            return s_normal_texture;
        }
        Vector<uint32_t> data;
        data.reserve(FOW_TEXTURE_PLACEHOLDER_SIZE * FOW_TEXTURE_PLACEHOLDER_SIZE);
        for (int i = 0; i < FOW_TEXTURE_PLACEHOLDER_SIZE * FOW_TEXTURE_PLACEHOLDER_SIZE; ++i) {
            data.emplace_back(0xFFFF7F7F);
        }

        GLuint id;
        glGenTextures(1, &id);
        if (id == 0) {
            throw std::runtime_error(std::format("Failed to generate default normal texture: GL error {}", glGetError()));
        }

        glBindTexture(GL_TEXTURE_2D, id);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, FOW_TEXTURE_PLACEHOLDER_SIZE, FOW_TEXTURE_PLACEHOLDER_SIZE, 0, GL_RGBA, GL_UNSIGNED_BYTE, data.data());
        glTextureParameteri(id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTextureParameteri(id, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(id, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTextureParameteri(id, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glBindTexture(GL_TEXTURE_2D, 0);
        s_normal_texture = std::make_shared<Texture2D>(std::move(Texture2D { id }));
        return s_normal_texture;
    }

    void Texture::UnloadPlaceHolder() {
        if (s_placeholder_texture != nullptr) {
            s_placeholder_texture = nullptr;
        }
        if (s_white_texture != nullptr) {
            s_white_texture = nullptr;
        }
        if (s_black_texture != nullptr) {
            s_black_texture = nullptr;
        }
        if (s_normal_texture != nullptr) {
            s_normal_texture = nullptr;
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

        glCreateTextures(gl_target, 1, &id);

        if (id == 0) {
            result = Failure(std::format("Failed to generate OpenGL texture handle: GL error \"{}\"", glGetError()));
            goto LOAD_GL_TEXTURE_END;
        }

        if (target == TextureTarget::TextureCubeMap) {
            id = SOIL_load_OGL_single_cubemap_from_memory(data.data(), data.size(), SOIL_DDS_CUBEMAP_FACE_ORDER, SOIL_LOAD_AUTO, id, 0);
        } else if (target == TextureTarget::Texture2D) {
            id = SOIL_load_OGL_texture_from_memory(data.data(), data.size(), SOIL_LOAD_AUTO, id, 0);
        } else if (target == TextureTarget::Texture2DArray) {
            id = SOIL_load_OGL_texture_array_from_atlas_grid_from_memory(data.data(), data.size(), frame_count, 1, SOIL_LOAD_AUTO, id, 0);
        } else {
            result = Failure("Failed to load OpenGL texture data: Unsupported texture target");
            goto LOAD_GL_TEXTURE_END;
        }

        glBindTexture(gl_target, id);
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
        return result;
    }

    Result<Texture2DPtr> Texture2D::Load(const TextureInfo& info) {
        const auto image_data = Assets::LoadAsBytes(info.Source.c_str());
        if (!image_data.has_value()) {
            return Failure(std::format("Failed to load image \"{}\": Image asset doesn't exists!", info.Source));
        }
        return LoadFromMemory(image_data.value(), info);
    }
    Result<Texture2DPtr> Texture2D::LoadFromMemory(const Vector<uint8_t>& data, const TextureInfo& info) {
        auto id_result = LoadOpenGLTexture(data, info);
        if (id_result.has_value()) {
            return Success<Texture2DPtr>(std::make_shared<Texture2D>(std::move(Texture2D { id_result.value() })));
        }
        return Failure(std::format("Failed to load image data \"{}\": {}", info.Source, id_result.error().message), id_result.error().location);
    }

    Result<Texture2DPtr> Texture2D::LoadAsset(const Path& path, const AssetLoaderFlags::Type flags) {
        if (!path.extension().equals(".xml", StringCompareType::CaseInsensitive)) {
            return Failure(std::format("Failed to load texture \"{}\": Expected asset extension '.xml'", path));
        }
        const auto xml_str = Assets::LoadAsString(path, flags);
        if (!xml_str.has_value()) {
            return Failure(std::format("Failed to load texture \"{}\": {}", path, xml_str.error().message));
        }
        const auto info_value = ParseTextureInfo(xml_str.value().as_std_str());
        if (!info_value.has_value()) {
            return Failure(std::format("Failed to load texture \"{}\": {}", path, info_value.error().message));
        }
        if (info_value->Target.value_or(TextureTarget::Texture2D) != TextureTarget::Texture2D) {
            return Failure(std::format("Failed to load texture \"{}\": Expected target 'Texture2D'", path));
        }
        if (const auto image_data = Assets::LoadAsBytes(info_value->Source.c_str(), flags); image_data.has_value()) {
            return LoadFromMemory(image_data.value(), info_value.value());
        }
        return Failure(std::format("Failed to load texture \"{}\": Image data \"{}\" cannot be found!", path, info_value->Source));
    }

    Result<Texture2DArrayPtr> Texture2DArray::Load(const TextureInfo& info) {
        const auto image_data = Assets::LoadAsBytes(info.Source.c_str());
        if (!image_data.has_value()) {
            return Failure(std::format("Failed to load image \"{}\": Image asset doesn't exists!", info.Source));
        }
        return LoadFromMemory(image_data.value(), info);
    }
    Result<Texture2DArrayPtr> Texture2DArray::LoadFromMemory(const Vector<uint8_t>& data, const TextureInfo& info) {
        auto id_result = LoadOpenGLTexture(data, info);
        if (id_result.has_value()) {
            return Success<Texture2DArrayPtr>(std::make_shared<Texture2DArray>(std::move(Texture2DArray { id_result.value() })));
        }
        return Failure(std::format("Failed to load image data \"{}\": {}", info.Source, id_result.error().message), id_result.error().location);
    }

    Result<Texture2DArrayPtr> Texture2DArray::LoadAsset(const Path& path, const AssetLoaderFlags::Type flags) {
        if (!path.extension().equals(".xml", StringCompareType::CaseInsensitive)) {
            return Failure(std::format("Failed to load texture \"{}\": Expected asset extension '.xml'", path));
        }
        const auto xml_str = Assets::LoadAsString(path, flags);
        if (!xml_str.has_value()) {
            return Failure(std::format("Failed to load texture \"{}\": {}", path, xml_str.error().message));
        }
        const auto info_value = ParseTextureInfo(xml_str.value().as_std_str());
        if (!info_value.has_value()) {
            return Failure(std::format("Failed to load texture \"{}\": {}", path, info_value.error().message));
        }
        if (info_value->Target.value_or(TextureTarget::Texture2DArray) != TextureTarget::Texture2DArray) {
            return Failure(std::format("Failed to load texture \"{}\": Expected target 'Texture2DArray'", path));
        }
        if (const auto image_data = Assets::LoadAsBytes(info_value->Source.c_str(), flags); image_data.has_value()) {
            return LoadFromMemory(image_data.value(), info_value.value());
        }
        return Failure(std::format("Failed to load texture \"{}\": Image data \"{}\" cannot be found!", path, info_value->Source));
    }

    Result<TextureCubeMapPtr> TextureCubeMap::Load(const TextureInfo& info) {
        const auto image_data = Assets::LoadAsBytes(info.Source.c_str());
        if (!image_data.has_value()) {
            return Failure(std::format("Failed to load image \"{}\": Image asset doesn't exists!", info.Source));
        }
        return LoadFromMemory(image_data.value(), info);
    }
    Result<TextureCubeMapPtr> TextureCubeMap::LoadFromMemory(const Vector<uint8_t>& data, const TextureInfo& info) {
        auto id_result = LoadOpenGLTexture(data, info);
        if (id_result.has_value()) {
            return Success<TextureCubeMapPtr>(std::make_shared<TextureCubeMap>(std::move(TextureCubeMap { id_result.value() })));
        }
        return Failure(std::format("Failed to load image data \"{}\": {}", info.Source, id_result.error().message), id_result.error().location);
    }

    Result<TextureCubeMapPtr> TextureCubeMap::LoadAsset(const Path& path, const AssetLoaderFlags::Type flags) {
        if (!path.extension().equals(".xml", StringCompareType::CaseInsensitive)) {
            return Failure(std::format("Failed to load texture \"{}\": Expected asset extension '.xml'", path));
        }
        const auto xml_str = Assets::LoadAsString(path, flags);
        if (!xml_str.has_value()) {
            return Failure(std::format("Failed to load texture \"{}\": {}", path, xml_str.error().message));
        }
        const auto info_value = ParseTextureInfo(xml_str.value().as_std_str());
        if (!info_value.has_value()) {
            return Failure(std::format("Failed to load texture \"{}\": {}", path, info_value.error().message));
        }
        if (info_value->Target.value_or(TextureTarget::TextureCubeMap) != TextureTarget::TextureCubeMap) {
            return Failure(std::format("Failed to load texture \"{}\": Expected target 'TextureCubeMap'", path));
        }
        if (const auto image_data = Assets::LoadAsBytes(info_value->Source.c_str(), flags); image_data.has_value()) {
            return LoadFromMemory(image_data.value(), info_value.value());
        }
        return Failure(std::format("Failed to load texture \"{}\": Image data \"{}\" cannot be found!", path, info_value->Source));
    }
}
