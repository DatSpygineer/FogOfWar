#ifndef FOW_EDITOR_SHADER_PARAM_INFO_HPP
#define FOW_EDITOR_SHADER_PARAM_INFO_HPP

#include <fow/Shared.hpp>
#include <fow/Renderer.hpp>

namespace fow {
    enum class ShaderParamType {
        Bool,
        Int,
        UInt,
        Float,
        IntVector2,
        IntVector3,
        IntVector4,
        UIntVector2,
        UIntVector3,
        UIntVector4,
        FloatVector2,
        FloatVector3,
        FloatVector4,
        Color,
        Matrix4,
        Texture
    };

    using ShaderParamValues = std::variant<
        bool,
        GLint,
        GLuint,
        GLfloat,
        Vector2i,
        Vector3i,
        Vector4i,
        Vector2u,
        Vector3u,
        Vector4u,
        Vector2,
        Vector3,
        Vector4,
        Color,
        Matrix4,
        String
    >;

    struct ShaderParamInfo {
        String name, category;
        ShaderParamType type;
        String defaultValue;
    };

    class ShaderParams : public HashMap<String, ShaderParamInfo> {
    public:
        ShaderParams() = default;

        static Result<ShaderParams> Load(const String& name);
        static Result<ShaderParams> FromXml(const String& name, const pugi::xml_node& node);
    };
}

#endif