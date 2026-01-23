#ifndef FOW_RENDERER_SHADER_HPP
#define FOW_RENDERER_SHADER_HPP

#include <glad/glad.h>

#include <variant>
#include <glm/glm.hpp>
#include <pugixml.hpp>

#include "fow/Shared.hpp"
#include "fow/String.hpp"
#include "fow/Assets.hpp"
#include "fow/Debug.hpp"
#include "fow/Renderer/Texture.hpp"

#define FOW_SHADER_PLACEHOLDER_ASSET_PATH "FOGOFWAR::Shaders/Null"

namespace fow {
    enum class ShaderUniformType {
        Bool             = GL_BOOL,
        BoolVector2      = GL_BOOL_VEC2,
        BoolVector3      = GL_BOOL_VEC3,
        BoolVector4      = GL_BOOL_VEC4,
        Int              = GL_INT,
        IntVector2       = GL_INT_VEC2,
        IntVector3       = GL_INT_VEC3,
        IntVector4       = GL_INT_VEC4,
        UInt             = GL_UNSIGNED_INT,
        UIntVector2      = GL_UNSIGNED_INT_VEC2,
        UIntVector3      = GL_UNSIGNED_INT_VEC3,
        UIntVector4      = GL_UNSIGNED_INT_VEC4,
        Float            = GL_FLOAT,
        FloatVector2     = GL_FLOAT_VEC2,
        FloatVector3     = GL_FLOAT_VEC3,
        FloatVector4     = GL_FLOAT_VEC4,
        Matrix4          = GL_FLOAT_MAT4,
        Double           = GL_DOUBLE,
        DoubleVector2    = GL_DOUBLE_VEC2,
        DoubleVector3    = GL_DOUBLE_VEC3,
        DoubleVector4    = GL_DOUBLE_VEC4,
        Sampler1D        = GL_SAMPLER_1D,
        Sampler2D        = GL_SAMPLER_2D,
        Sampler3D        = GL_SAMPLER_3D,
        SamplerCube      = GL_SAMPLER_CUBE,
        Sampler1DArray   = GL_SAMPLER_1D_ARRAY,
        Sampler2DArray   = GL_SAMPLER_2D_ARRAY,
        SamplerCubeArray = GL_SAMPLER_CUBE_MAP_ARRAY
    };

    struct FOW_RENDER_API ShaderUniformInfo {
        String name;
        GLint location;
        ShaderUniformType type;
    };

    class Shader;

    using ShaderPtr = SharedPtr<Shader>;
    using ShaderPtr = SharedPtr<Shader>;

    class FOW_RENDER_API Shader final {
        GLuint m_uProgram;
        bool   m_bInitialized;
        bool   m_bBackfaceCulling = true;
        bool   m_bOpaque          = true;

        explicit Shader(const GLuint id) : m_uProgram(id), m_bInitialized(true) { }
    public:
        Shader() : m_uProgram(0),  m_bInitialized(false) { }
        Shader(const Shader& other) = delete;
        Shader(Shader&& other) noexcept : m_uProgram(other.m_uProgram), m_bInitialized(other.m_bInitialized) {
            other.m_uProgram = 0;
            other.m_bInitialized = false;
        }
        ~Shader();

        Shader& operator=(const Shader& other) {
            if (m_uProgram != 0 && m_bInitialized) {
                glDeleteProgram(m_uProgram);
            }
            m_uProgram = other.m_uProgram;
            m_bInitialized = other.m_bInitialized;
            return *this;
        }
        Shader& operator=(Shader&& other) noexcept {
            if (this != &other) {
                if (m_uProgram != 0 && m_bInitialized) {
                    glDeleteProgram(m_uProgram);
                }
                m_uProgram = other.m_uProgram;
                m_bInitialized = other.m_bInitialized;
                other.m_uProgram = 0;
                other.m_bInitialized = false;
            }
            return *this;
        }

        [[nodiscard]] constexpr GLuint id() const { return m_uProgram; }
        [[nodiscard]] constexpr bool is_valid() const { return m_uProgram != 0; }

        void use() const;
        bool set_uniform(const String& name, bool value)               const; // NOLINT: Return type is not always used.
        bool set_uniform(const String& name, GLint value)              const; // NOLINT: Return type is not always used.
        bool set_uniform(const String& name, GLuint value)             const; // NOLINT: Return type is not always used.
        bool set_uniform(const String& name, GLfloat value)            const; // NOLINT: Return type is not always used.
        bool set_uniform(const String& name, GLdouble value)           const; // NOLINT: Return type is not always used.
        bool set_uniform(const String& name, const glm::bvec2& value)  const; // NOLINT: Return type is not always used.
        bool set_uniform(const String& name, const glm::bvec3& value)  const; // NOLINT: Return type is not always used.
        bool set_uniform(const String& name, const glm::bvec4& value)  const; // NOLINT: Return type is not always used.
        bool set_uniform(const String& name, const glm::ivec2& value)  const; // NOLINT: Return type is not always used.
        bool set_uniform(const String& name, const glm::ivec3& value)  const; // NOLINT: Return type is not always used.
        bool set_uniform(const String& name, const glm::ivec4& value)  const; // NOLINT: Return type is not always used.
        bool set_uniform(const String& name, const glm::uvec2& value)  const; // NOLINT: Return type is not always used.
        bool set_uniform(const String& name, const glm::uvec3& value)  const; // NOLINT: Return type is not always used.
        bool set_uniform(const String& name, const glm::uvec4& value)  const; // NOLINT: Return type is not always used.
        bool set_uniform(const String& name, const glm::vec2& value)   const; // NOLINT: Return type is not always used.
        bool set_uniform(const String& name, const glm::vec3& value)   const; // NOLINT: Return type is not always used.
        bool set_uniform(const String& name, const glm::vec4& value)   const; // NOLINT: Return type is not always used.
        bool set_uniform(const String& name, const glm::dvec2& value)  const; // NOLINT: Return type is not always used.
        bool set_uniform(const String& name, const glm::dvec3& value)  const; // NOLINT: Return type is not always used.
        bool set_uniform(const String& name, const glm::dvec4& value)  const; // NOLINT: Return type is not always used.
        bool set_uniform(const String& name, const glm::mat4& value)   const; // NOLINT: Return type is not always used.

        bool set_uniform(const String& name, const Vector<bool>& values)       const; // NOLINT: Return type is not always used.
        bool set_uniform(const String& name, const Vector<GLint>& values)      const; // NOLINT: Return type is not always used.
        bool set_uniform(const String& name, const Vector<GLuint>& values)     const; // NOLINT: Return type is not always used.
        bool set_uniform(const String& name, const Vector<GLfloat>& values)    const; // NOLINT: Return type is not always used.
        bool set_uniform(const String& name, const Vector<GLdouble>& values)   const; // NOLINT: Return type is not always used.
        bool set_uniform(const String& name, const Vector<glm::ivec2>& values) const; // NOLINT: Return type is not always used.
        bool set_uniform(const String& name, const Vector<glm::ivec3>& values) const; // NOLINT: Return type is not always used.
        bool set_uniform(const String& name, const Vector<glm::ivec4>& values) const; // NOLINT: Return type is not always used.
        bool set_uniform(const String& name, const Vector<glm::uvec2>& values) const; // NOLINT: Return type is not always used.
        bool set_uniform(const String& name, const Vector<glm::uvec3>& values) const; // NOLINT: Return type is not always used.
        bool set_uniform(const String& name, const Vector<glm::uvec4>& values) const; // NOLINT: Return type is not always used.
        bool set_uniform(const String& name, const Vector<glm::vec2>& values)  const; // NOLINT: Return type is not always used.
        bool set_uniform(const String& name, const Vector<glm::vec3>& values)  const; // NOLINT: Return type is not always used.
        bool set_uniform(const String& name, const Vector<glm::vec4>& values)  const; // NOLINT: Return type is not always used.
        bool set_uniform(const String& name, const Vector<glm::dvec2>& values) const; // NOLINT: Return type is not always used.
        bool set_uniform(const String& name, const Vector<glm::dvec3>& values) const; // NOLINT: Return type is not always used.
        bool set_uniform(const String& name, const Vector<glm::dvec4>& values) const; // NOLINT: Return type is not always used.
        bool set_uniform(const String& name, const Vector<glm::mat4>& values)  const; // NOLINT: Return type is not always used.

        void set_uniform(GLint location, bool value)              const;
        void set_uniform(GLint location, GLint value)             const;
        void set_uniform(GLint location, GLuint value)            const;
        void set_uniform(GLint location, GLfloat value)           const;
        void set_uniform(GLint location, GLdouble value)          const;
        void set_uniform(GLint location, const glm::bvec2& value) const;
        void set_uniform(GLint location, const glm::bvec3& value) const;
        void set_uniform(GLint location, const glm::bvec4& value) const;
        void set_uniform(GLint location, const glm::ivec2& value) const;
        void set_uniform(GLint location, const glm::ivec3& value) const;
        void set_uniform(GLint location, const glm::ivec4& value) const;
        void set_uniform(GLint location, const glm::uvec2& value) const;
        void set_uniform(GLint location, const glm::uvec3& value) const;
        void set_uniform(GLint location, const glm::uvec4& value) const;
        void set_uniform(GLint location, const glm::vec2& value)  const;
        void set_uniform(GLint location, const glm::vec3& value)  const;
        void set_uniform(GLint location, const glm::vec4& value)  const;
        void set_uniform(GLint location, const glm::dvec2& value) const;
        void set_uniform(GLint location, const glm::dvec3& value) const;
        void set_uniform(GLint location, const glm::dvec4& value) const;
        void set_uniform(GLint location, const glm::mat4& value)  const;

        void set_uniform(GLint location, const Vector<bool>& values)       const;
        void set_uniform(GLint location, const Vector<GLint>& values)      const;
        void set_uniform(GLint location, const Vector<GLuint>& values)     const;
        void set_uniform(GLint location, const Vector<GLfloat>& values)    const;
        void set_uniform(GLint location, const Vector<GLdouble>& values)   const;
        void set_uniform(GLint location, const Vector<glm::ivec2>& values) const;
        void set_uniform(GLint location, const Vector<glm::ivec3>& values) const;
        void set_uniform(GLint location, const Vector<glm::ivec4>& values) const;
        void set_uniform(GLint location, const Vector<glm::uvec2>& values) const;
        void set_uniform(GLint location, const Vector<glm::uvec3>& values) const;
        void set_uniform(GLint location, const Vector<glm::uvec4>& values) const;
        void set_uniform(GLint location, const Vector<glm::vec2>& values)  const;
        void set_uniform(GLint location, const Vector<glm::vec3>& values)  const;
        void set_uniform(GLint location, const Vector<glm::vec4>& values)  const;
        void set_uniform(GLint location, const Vector<glm::dvec2>& values) const;
        void set_uniform(GLint location, const Vector<glm::dvec3>& values) const;
        void set_uniform(GLint location, const Vector<glm::dvec4>& values) const;
        void set_uniform(GLint location, const Vector<glm::mat4>& values)  const;

        bool get_uniform(const String& name, bool& value)           const;
        bool get_uniform(const String& name, GLint& value)          const;
        bool get_uniform(const String& name, GLuint& value)         const;
        bool get_uniform(const String& name, GLfloat& value)        const;
        bool get_uniform(const String& name, GLdouble& value)       const;
        bool get_uniform(const String& name, glm::bvec2& value)     const;
        bool get_uniform(const String& name, glm::bvec3& value)     const;
        bool get_uniform(const String& name, glm::bvec4& value)     const;
        bool get_uniform(const String& name, glm::ivec2& value)     const;
        bool get_uniform(const String& name, glm::ivec3& value)     const;
        bool get_uniform(const String& name, glm::ivec4& value)     const;
        bool get_uniform(const String& name, glm::uvec2& value)     const;
        bool get_uniform(const String& name, glm::uvec3& value)     const;
        bool get_uniform(const String& name, glm::uvec4& value)     const;
        bool get_uniform(const String& name, glm::vec2& value)      const;
        bool get_uniform(const String& name, glm::vec3& value)      const;
        bool get_uniform(const String& name, glm::vec4& value)      const;
        bool get_uniform(const String& name, glm::dvec2& value)     const;
        bool get_uniform(const String& name, glm::dvec3& value)     const;
        bool get_uniform(const String& name, glm::dvec4& value)     const;
        bool get_uniform(const String& name, glm::mat4& value)      const;

        void set_opaque(bool value);
        constexpr bool get_opaque() const { return m_bOpaque; }
        void set_backface_culling(bool value);
        constexpr bool get_backface_culling() const { return m_bBackfaceCulling; }

        [[nodiscard]] GLint uniform_location(const String& name) const;
        [[nodiscard]] inline bool has_uniform(const String& name) const { return uniform_location(name) >= 0; }

        [[nodiscard]] Result<ShaderUniformInfo> get_uniform_info(const String& name) const;
        [[nodiscard]] Result<ShaderUniformInfo> get_uniform_info(GLint location) const;
        [[nodiscard]] size_t get_uniform_count() const;

        static Result<ShaderPtr> Compile(const String& name, const String& vertex, const String& fragment);
        static Result<ShaderPtr> FromBinary(const String& name, const void* data, size_t data_size, const String& vertex_entry, const String& fragment_entry);
        static Result<ShaderPtr> FromBinary(const String& name,
            const void* vertex_data,   size_t vertex_data_size,
            const void* fragment_data, size_t fragment_data_size,
            const String& vertex_entry, const String& fragment_entry
        );
        static Result<ShaderPtr> LoadAsset(const Path& path, AssetLoaderFlags::Type flags);

        static ShaderPtr PlaceHolder();
        static void UnloadPlaceHolder();
    };
}

template<>
struct std::formatter<fow::ShaderUniformType> : std::formatter<std::string_view> {
    auto format(const fow::ShaderUniformType type, std::format_context& ctx) const {
        std::string_view name;
        switch (type) {
            case fow::ShaderUniformType::Bool:             name = "bool";             break;
            case fow::ShaderUniformType::BoolVector2:      name = "bvec2";            break;
            case fow::ShaderUniformType::BoolVector3:      name = "bvec3";            break;
            case fow::ShaderUniformType::BoolVector4:      name = "bvec4";            break;
            case fow::ShaderUniformType::UInt:             name = "uint";             break;
            case fow::ShaderUniformType::UIntVector2:      name = "uvec2";            break;
            case fow::ShaderUniformType::UIntVector3:      name = "uvec3";            break;
            case fow::ShaderUniformType::UIntVector4:      name = "uvec4";            break;
            case fow::ShaderUniformType::Int:              name = "int";              break;
            case fow::ShaderUniformType::IntVector2:       name = "ivec2";            break;
            case fow::ShaderUniformType::IntVector3:       name = "ivec3";            break;
            case fow::ShaderUniformType::IntVector4:       name = "ivec4";            break;
            case fow::ShaderUniformType::Float:            name = "float";            break;
            case fow::ShaderUniformType::FloatVector2:     name = "vec2";             break;
            case fow::ShaderUniformType::FloatVector3:     name = "vec3";             break;
            case fow::ShaderUniformType::FloatVector4:     name = "vec4";             break;
            case fow::ShaderUniformType::Double:           name = "double";           break;
            case fow::ShaderUniformType::DoubleVector2:    name = "dvec2";            break;
            case fow::ShaderUniformType::DoubleVector3:    name = "dvec3";            break;
            case fow::ShaderUniformType::DoubleVector4:    name = "dvec4";            break;
            case fow::ShaderUniformType::Matrix4:          name = "mat4";             break;
            case fow::ShaderUniformType::Sampler1D:        name = "sampler1D";        break;
            case fow::ShaderUniformType::Sampler1DArray:   name = "sampler1DArray";   break;
            case fow::ShaderUniformType::Sampler2D:        name = "sampler2D";        break;
            case fow::ShaderUniformType::Sampler2DArray:   name = "sampler2DArray";   break;
            case fow::ShaderUniformType::Sampler3D:        name = "sampler3D";        break;
            case fow::ShaderUniformType::SamplerCube:      name = "SamplerCube";      break;
            case fow::ShaderUniformType::SamplerCubeArray: name = "SamplerCubeArray"; break;
        }
        return std::formatter<std::string_view>::format(name, ctx);
    }
};

#endif