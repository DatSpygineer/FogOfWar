#ifndef FOW_RENDERER_MATERIAL_HPP
#define FOW_RENDERER_MATERIAL_HPP

#include "fow/Shared.hpp"
#include "fow/String.hpp"
#include "fow/StringConvertion.hpp"
#include "fow/Assets.hpp"
#include "fow/Renderer/Shader.hpp"

namespace fow {
    using MaterialParameterValue = std::variant<
        bool,
        GLuint,
        GLint,
        GLfloat,
        GLdouble,
        glm::bvec2,
        glm::bvec3,
        glm::bvec4,
        glm::uvec2,
        glm::uvec3,
        glm::uvec4,
        glm::ivec2,
        glm::ivec3,
        glm::ivec4,
        glm::vec2,
        glm::vec3,
        glm::vec4,
        glm::dvec2,
        glm::dvec3,
        glm::dvec4,
        glm::mat4,
        TexturePtr
    >;

    enum class MaterialParameterType {
        Bool,
        UInt,
        Int,
        Float,
        Double,
        BoolVector2,
        BoolVector3,
        BoolVector4,
        UIntVector2,
        UIntVector3,
        UIntVector4,
        IntVector2,
        IntVector3,
        IntVector4,
        FloatVector2,
        FloatVector3,
        FloatVector4,
        DoubleVector2,
        DoubleVector3,
        DoubleVector4,
        Matrix4,
        Texture
    };

    class Material;
    using MaterialPtr = SharedPtr<Material>;

    class FOW_RENDER_API Material final : std::enable_shared_from_this<Material> {
        ShaderPtr m_pShader;
        HashMap<String, MaterialParameterValue> m_mParams;
    public:
        Material() : m_pShader(nullptr) { }
        explicit Material(const ShaderPtr& shader, const HashMap<String, MaterialParameterValue>& params = { }) :
            m_pShader(shader), m_mParams(params) { }
        explicit Material(ShaderPtr&& shader, const HashMap<String, MaterialParameterValue>& params = { }) :
            m_pShader(std::move(shader)), m_mParams(params) { }
        Material(const Material& material) = delete;
        Material(Material&& material) noexcept : m_pShader(std::move(material.m_pShader)), m_mParams(std::move(material.m_mParams)) {
            material.m_pShader = nullptr;
            material.m_mParams = { };
        }

        Material& operator=(const Material& material) = delete;
        Material& operator=(Material&& material) noexcept {
            m_pShader = material.m_pShader;
            m_mParams = material.m_mParams;
            material.m_pShader = nullptr;
            material.m_mParams = { };
            return *this;
        }

        [[nodiscard]] constexpr const ShaderPtr& shader() const { return m_pShader; }

        Result<> set_parameter(const String& name, const MaterialParameterValue& value);
        Result<> get_parameter(const String& name, MaterialParameterValue& value) const;

        Result<> apply() const;

        [[nodiscard]] constexpr bool is_valid() const { return m_pShader != nullptr; }

        static Result<MaterialPtr> ParseXml(const String& source_asset_path, const String& xml_src, AssetLoaderFlags::Type flags);
        static Result<MaterialPtr> ParseXml(const String& source, const pugi::xml_document& doc, AssetLoaderFlags::Type flags);
        static Result<MaterialPtr> ParseXml(const String& source, const pugi::xml_node& root, AssetLoaderFlags::Type flags);
        static Result<MaterialPtr> LoadAsset(const Path& path, AssetLoaderFlags::Type flags);

        static const Material Null;
    };
}

#endif