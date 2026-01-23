#include "fow/Renderer/Shader.hpp"

#include <glm/gtc/type_ptr.hpp>

#include "fow/StringConvertion.hpp"

namespace fow {
    static HashMap<String, String> s_built_in_shader_headers = { };

    static Result<String> ResolveShaderIncludes(const String& name, const String& source) {
        std::istringstream input(source.as_std_str());
        std::string line;
        std::ostringstream result;
        size_t line_i = 1;

        while (std::getline(input, line)) {
            if (auto s_line = String(line); s_line.clone_trimmed().starts_with("#include")) {
                auto asset_path = s_line.clone_trimmed();
                asset_path.trim("\"").trim_begin("<").trim_end(">");
                result << "#line 1 \"" << asset_path.as_std_str() << "\"" << std::endl;
                String include_source;
                if (s_built_in_shader_headers.contains(asset_path)) {
                    include_source = s_built_in_shader_headers.at(asset_path);
                } else {
                    const auto include_source_result = Assets::LoadAsString(asset_path);
                    if (!include_source_result.has_value()) {
                        return Failure<String>(include_source_result.error());
                    }
                    include_source = include_source_result.value();
                }
                line_i++;
                result << include_source.as_std_str() << std::endl << "#line " << line_i << " \"" << name << "\"" << std::endl;
            } else {
                result << line << std::endl;
                line_i++;
            }
        }
        return Success<String>(result.str());
    }

    Shader::~Shader() {
        if (m_uProgram != 0 && m_bInitialized) {
            glDeleteProgram(m_uProgram);
        }
    }

    void Shader::use() const {
        glUseProgram(m_uProgram);
    }

    bool Shader::set_uniform(const String& name, const bool value) const {
        const GLint loc = glGetUniformLocation(m_uProgram, name.as_cstr());
        if (loc < 0) {
            return false;
        }
        glUniform1ui(loc, value ? 1 : 0);
        return true;
    }
    bool Shader::set_uniform(const String& name, const GLint value) const {
        const GLint loc = glGetUniformLocation(m_uProgram, name.as_cstr());
        if (loc < 0) {
            return false;
        }
        glUniform1i(loc, value);
        return true;
    }
    bool Shader::set_uniform(const String& name, const GLuint value) const {
        const GLint loc = glGetUniformLocation(m_uProgram, name.as_cstr());
        if (loc < 0) {
            return false;
        }
        glUniform1ui(loc, value);
        return true;
    }
    bool Shader::set_uniform(const String& name, const GLfloat value) const {
        const GLint loc = glGetUniformLocation(m_uProgram, name.as_cstr());
        if (loc < 0) {
            return false;
        }
        glUniform1f(loc, value);
        return true;
    }
    bool Shader::set_uniform(const String& name, const GLdouble value) const {
        const GLint loc = glGetUniformLocation(m_uProgram, name.as_cstr());
        if (loc < 0) {
            return false;
        }
        glUniform1d(loc, value);
        return true;
    }
    bool Shader::set_uniform(const String& name, const glm::bvec2& value) const {
        const GLint loc = glGetUniformLocation(m_uProgram, name.as_cstr());
        if (loc < 0) {
            return false;
        }
        glUniform2ui(loc, value.x ? 1 : 0, value.y ? 1 : 0);
        return true;
    }
    bool Shader::set_uniform(const String& name, const glm::bvec3& value) const {
        const GLint loc = glGetUniformLocation(m_uProgram, name.as_cstr());
        if (loc < 0) {
            return false;
        }
        glUniform3ui(loc, value.x ? 1 : 0, value.y ? 1 : 0, value.z ? 1 : 0);
        return true;
    }
    bool Shader::set_uniform(const String& name, const glm::bvec4& value) const {
        const GLint loc = glGetUniformLocation(m_uProgram, name.as_cstr());
        if (loc < 0) {
            return false;
        }
        glUniform4ui(loc, value.x ? 1 : 0, value.y ? 1 : 0, value.z ? 1 : 0, value.w ? 1 : 0);
        return true;
    }
    bool Shader::set_uniform(const String& name, const glm::ivec2& value) const {
        const GLint loc = glGetUniformLocation(m_uProgram, name.as_cstr());
        if (loc < 0) {
            return false;
        }
        glUniform2i(loc, value.x, value.y);
        return true;
    }
    bool Shader::set_uniform(const String& name, const glm::ivec3& value) const {
        const GLint loc = glGetUniformLocation(m_uProgram, name.as_cstr());
        if (loc < 0) {
            return false;
        }
        glUniform3i(loc, value.x, value.y, value.z);
        return true;
    }
    bool Shader::set_uniform(const String& name, const glm::ivec4& value) const {
        const GLint loc = glGetUniformLocation(m_uProgram, name.as_cstr());
        if (loc < 0) {
            return false;
        }
        glUniform4i(loc, value.x, value.y, value.z, value.w);
        return true;
    }
    bool Shader::set_uniform(const String& name, const glm::uvec2& value) const {
        const GLint loc = glGetUniformLocation(m_uProgram, name.as_cstr());
        if (loc < 0) {
            return false;
        }
        glUniform2ui(loc, value.x, value.y);
        return true;
    }
    bool Shader::set_uniform(const String& name, const glm::uvec3& value) const {
        const GLint loc = glGetUniformLocation(m_uProgram, name.as_cstr());
        if (loc < 0) {
            return false;
        }
        glUniform3ui(loc, value.x, value.y, value.z);
        return true;
    }
    bool Shader::set_uniform(const String& name, const glm::uvec4& value) const {
        const GLint loc = glGetUniformLocation(m_uProgram, name.as_cstr());
        if (loc < 0) {
            return false;
        }
        glUniform4ui(loc, value.x, value.y, value.z, value.w);
        return true;
    }
    bool Shader::set_uniform(const String& name, const glm::vec2& value) const {
        const GLint loc = glGetUniformLocation(m_uProgram, name.as_cstr());
        if (loc < 0) {
            return false;
        }
        glUniform2f(loc, value.x, value.y);
        return true;
    }
    bool Shader::set_uniform(const String& name, const glm::vec3& value) const {
        const GLint loc = glGetUniformLocation(m_uProgram, name.as_cstr());
        if (loc < 0) {
            return false;
        }
        glUniform3f(loc, value.x, value.y, value.z);
        return true;
    }
    bool Shader::set_uniform(const String& name, const glm::vec4& value) const {
        const GLint loc = glGetUniformLocation(m_uProgram, name.as_cstr());
        if (loc < 0) {
            return false;
        }
        glUniform4f(loc, value.x, value.y, value.z, value.w);
        return true;
    }
    bool Shader::set_uniform(const String& name, const glm::dvec2& value) const {
        const GLint loc = glGetUniformLocation(m_uProgram, name.as_cstr());
        if (loc < 0) {
            return false;
        }
        glUniform2d(loc, value.x, value.y);
        return true;
    }
    bool Shader::set_uniform(const String& name, const glm::dvec3& value) const {
        const GLint loc = glGetUniformLocation(m_uProgram, name.as_cstr());
        if (loc < 0) {
            return false;
        }
        glUniform3d(loc, value.x, value.y, value.z);
        return true;
    }
    bool Shader::set_uniform(const String& name, const glm::dvec4& value) const {
        const GLint loc = glGetUniformLocation(m_uProgram, name.as_cstr());
        if (loc < 0) {
            return false;
        }
        glUniform4d(loc, value.x, value.y, value.z, value.w);
        return true;
    }
    bool Shader::set_uniform(const String& name, const glm::mat4& value) const {
        const GLint loc = glGetUniformLocation(m_uProgram, name.as_cstr());
        if (loc < 0) {
            return false;
        }
        glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(value));
        return true;
    }

    bool Shader::set_uniform(const String& name, const Vector<bool>& values) const {
        Vector<GLuint> uint_values(values.size());
        std::ranges::transform(values,
           uint_values.begin(),
           [](auto value) { return value ? 1u : 0u; }
        );
        return set_uniform(name, uint_values);
    }
    bool Shader::set_uniform(const String& name, const Vector<GLint>& values) const {
        const GLint loc = glGetUniformLocation(m_uProgram, name.as_cstr());
        if (loc < 0) {
            return false;
        }
        glUniform1iv(loc, values.size(), values.data());
        return true;
    }
    bool Shader::set_uniform(const String& name, const Vector<GLuint>& values) const {
        const GLint loc = glGetUniformLocation(m_uProgram, name.as_cstr());
        if (loc < 0) {
            return false;
        }
        glUniform1uiv(loc, values.size(), values.data());
        return true;
    }
    bool Shader::set_uniform(const String& name, const Vector<GLfloat>& values) const {
        const GLint loc = glGetUniformLocation(m_uProgram, name.as_cstr());
        if (loc < 0) {
            return false;
        }
        glUniform1fv(loc, values.size(), values.data());
        return true;
    }
    bool Shader::set_uniform(const String& name, const Vector<GLdouble>& values) const {
        const GLint loc = glGetUniformLocation(m_uProgram, name.as_cstr());
        if (loc < 0) {
            return false;
        }
        glUniform1dv(loc, values.size(), values.data());
        return true;
    }
    bool Shader::set_uniform(const String& name, const Vector<glm::ivec2>& values) const {
        const GLint loc = glGetUniformLocation(m_uProgram, name.as_cstr());
        if (loc < 0) {
            return false;
        }
        glUniform2iv(loc, values.size(), reinterpret_cast<const GLint*>(values.data()));
        return true;
    }
    bool Shader::set_uniform(const String& name, const Vector<glm::ivec3>& values) const {
        const GLint loc = glGetUniformLocation(m_uProgram, name.as_cstr());
        if (loc < 0) {
            return false;
        }
        glUniform3iv(loc, values.size(), reinterpret_cast<const GLint*>(values.data()));
        return true;
    }
    bool Shader::set_uniform(const String& name, const Vector<glm::ivec4>& values) const {
        const GLint loc = glGetUniformLocation(m_uProgram, name.as_cstr());
        if (loc < 0) {
            return false;
        }
        glUniform4iv(loc, values.size(), reinterpret_cast<const GLint*>(values.data()));
        return true;
    }
    bool Shader::set_uniform(const String& name, const Vector<glm::uvec2>& values) const {
        const GLint loc = glGetUniformLocation(m_uProgram, name.as_cstr());
        if (loc < 0) {
            return false;
        }
        glUniform2uiv(loc, values.size(), reinterpret_cast<const GLuint*>(values.data()));
        return true;
    }
    bool Shader::set_uniform(const String& name, const Vector<glm::uvec3>& values) const {
        const GLint loc = glGetUniformLocation(m_uProgram, name.as_cstr());
        if (loc < 0) {
            return false;
        }
        glUniform3uiv(loc, values.size(), reinterpret_cast<const GLuint*>(values.data()));
        return true;
    }
    bool Shader::set_uniform(const String& name, const Vector<glm::uvec4>& values) const {
        const GLint loc = glGetUniformLocation(m_uProgram, name.as_cstr());
        if (loc < 0) {
            return false;
        }
        glUniform4uiv(loc, values.size(), reinterpret_cast<const GLuint*>(values.data()));
        return true;
    }
    bool Shader::set_uniform(const String& name, const Vector<glm::vec2>& values) const {
        const GLint loc = glGetUniformLocation(m_uProgram, name.as_cstr());
        if (loc < 0) {
            return false;
        }
        glUniform2fv(loc, values.size(), reinterpret_cast<const GLfloat*>(values.data()));
        return true;
    }
    bool Shader::set_uniform(const String& name, const Vector<glm::vec3>& values) const {
        const GLint loc = glGetUniformLocation(m_uProgram, name.as_cstr());
        if (loc < 0) {
            return false;
        }
        glUniform3fv(loc, values.size(), reinterpret_cast<const GLfloat*>(values.data()));
        return true;
    }
    bool Shader::set_uniform(const String& name, const Vector<glm::vec4>& values) const {
        const GLint loc = glGetUniformLocation(m_uProgram, name.as_cstr());
        if (loc < 0) {
            return false;
        }
        glUniform4fv(loc, values.size(), reinterpret_cast<const GLfloat*>(values.data()));
        return true;
    }
    bool Shader::set_uniform(const String& name, const Vector<glm::dvec2>& values) const {
        const GLint loc = glGetUniformLocation(m_uProgram, name.as_cstr());
        if (loc < 0) {
            return false;
        }
        glUniform2dv(loc, values.size(), reinterpret_cast<const GLdouble*>(values.data()));
        return true;
    }
    bool Shader::set_uniform(const String& name, const Vector<glm::dvec3>& values) const {
        const GLint loc = glGetUniformLocation(m_uProgram, name.as_cstr());
        if (loc < 0) {
            return false;
        }
        glUniform3dv(loc, values.size(), reinterpret_cast<const GLdouble*>(values.data()));
        return true;
    }
    bool Shader::set_uniform(const String& name, const Vector<glm::dvec4>& values) const {
        const GLint loc = glGetUniformLocation(m_uProgram, name.as_cstr());
        if (loc < 0) {
            return false;
        }
        glUniform4dv(loc, values.size(), reinterpret_cast<const GLdouble*>(values.data()));
        return true;
    }
    bool Shader::set_uniform(const String& name, const Vector<glm::mat4>& values) const {
        const GLint loc = glGetUniformLocation(m_uProgram, name.as_cstr());
        if (loc < 0) {
            return false;
        }

        Vector<GLfloat> float_data(values.size() * 16);
        for (const auto& value : values) {
            const auto& p = glm::value_ptr(value);
            for (uint8_t i = 0; i < 16; ++i) {
                float_data.emplace_back(p[i]);
            }
        }

        glUniformMatrix4fv(loc, values.size(), GL_FALSE, float_data.data());
        return true;
    }

    void Shader::set_uniform(const GLint location, const bool value) const {
        glUniform1ui(location, value ? 1 : 0);
    }
    void Shader::set_uniform(const GLint location, const GLint value) const {
        glUniform1i(location, value);
    }
    void Shader::set_uniform(const GLint location, const GLuint value) const {
        glUniform1ui(location, value);
    }
    void Shader::set_uniform(const GLint location, const GLfloat value) const {
        glUniform1f(location, value);
    }
    void Shader::set_uniform(const GLint location, const GLdouble value) const {
        glUniform1d(location, value);
    }
    void Shader::set_uniform(const GLint location, const glm::bvec2& value) const {
        glUniform2ui(location, value.x ? 1 : 0, value.y ? 1 : 0);
    }
    void Shader::set_uniform(const GLint location, const glm::bvec3& value) const {
        glUniform3ui(location, value.x ? 1 : 0, value.y ? 1 : 0, value.z ? 1 : 0);
    }
    void Shader::set_uniform(const GLint location, const glm::bvec4& value) const {
        glUniform4ui(location, value.x ? 1 : 0, value.y ? 1 : 0, value.z ? 1 : 0, value.w ? 1 : 0);
    }
    void Shader::set_uniform(const GLint location, const glm::ivec2& value) const {
        glUniform2i(location, value.x, value.y);
    }
    void Shader::set_uniform(const GLint location, const glm::ivec3& value) const {
        glUniform3i(location, value.x, value.y, value.z);
    }
    void Shader::set_uniform(const GLint location, const glm::ivec4& value) const {
        glUniform4i(location, value.x, value.y, value.z, value.w);
    }
    void Shader::set_uniform(const GLint location, const glm::uvec2& value) const {
        glUniform2ui(location, value.x, value.y);
    }
    void Shader::set_uniform(const GLint location, const glm::uvec3& value) const {
        glUniform3ui(location, value.x, value.y, value.z);
    }
    void Shader::set_uniform(const GLint location, const glm::uvec4& value) const {
        glUniform4ui(location, value.x, value.y, value.z, value.w);
    }
    void Shader::set_uniform(const GLint location, const glm::vec2& value) const {
        glUniform2f(location, value.x, value.y);
    }
    void Shader::set_uniform(const GLint location, const glm::vec3& value) const {
        glUniform3f(location, value.x, value.y, value.z);
    }
    void Shader::set_uniform(const GLint location, const glm::vec4& value) const {
        glUniform4f(location, value.x, value.y, value.z, value.w);
    }
    void Shader::set_uniform(const GLint location, const glm::dvec2& value) const {
        glUniform2d(location, value.x, value.y);
    }
    void Shader::set_uniform(const GLint location, const glm::dvec3& value) const {
        glUniform3d(location, value.x, value.y, value.z);
    }
    void Shader::set_uniform(const GLint location, const glm::dvec4& value) const {
        glUniform4d(location, value.x, value.y, value.z, value.w);
    }
    void Shader::set_uniform(const GLint location, const glm::mat4& value) const {
        glUniformMatrix4fv(location, 16, GL_TRUE, &value[0][0]);
    }

    void Shader::set_uniform(const GLint location, const Vector<bool>& values) const {
        Vector<GLuint> uint_values(values.size());
        std::ranges::transform(values,
           uint_values.begin(),
           [](auto value) { return value ? 1u : 0u; }
        );
        set_uniform(location, uint_values);
    }
    void Shader::set_uniform(const GLint location, const Vector<GLint>& values) const {
        glUniform1iv(location, values.size(), values.data());
    }
    void Shader::set_uniform(const GLint location, const Vector<GLuint>& values) const {
        glUniform1uiv(location, values.size(), values.data());
    }
    void Shader::set_uniform(const GLint location, const Vector<GLfloat>& values) const {
        glUniform1fv(location, values.size(), values.data());
    }
    void Shader::set_uniform(const GLint location, const Vector<GLdouble>& values) const {
        glUniform1dv(location, values.size(), values.data());
    }
    void Shader::set_uniform(const GLint location, const Vector<glm::ivec2>& values) const {
        glUniform2iv(location, values.size(), reinterpret_cast<const GLint*>(values.data()));
    }
    void Shader::set_uniform(const GLint location, const Vector<glm::ivec3>& values) const {
        glUniform3iv(location, values.size(), reinterpret_cast<const GLint*>(values.data()));
    }
    void Shader::set_uniform(const GLint location, const Vector<glm::ivec4>& values) const {
        glUniform4iv(location, values.size(), reinterpret_cast<const GLint*>(values.data()));
    }
    void Shader::set_uniform(const GLint location, const Vector<glm::uvec2>& values) const {
        glUniform2uiv(location, values.size(), reinterpret_cast<const GLuint*>(values.data()));
    }
    void Shader::set_uniform(const GLint location, const Vector<glm::uvec3>& values) const {
        glUniform3uiv(location, values.size(), reinterpret_cast<const GLuint*>(values.data()));
    }
    void Shader::set_uniform(const GLint location, const Vector<glm::uvec4>& values) const {
        glUniform4uiv(location, values.size(), reinterpret_cast<const GLuint*>(values.data()));
    }
    void Shader::set_uniform(const GLint location, const Vector<glm::vec2>& values) const {
        glUniform2fv(location, values.size(), reinterpret_cast<const GLfloat*>(values.data()));
    }
    void Shader::set_uniform(const GLint location, const Vector<glm::vec3>& values) const {
        glUniform3fv(location, values.size(), reinterpret_cast<const GLfloat*>(values.data()));
    }
    void Shader::set_uniform(const GLint location, const Vector<glm::vec4>& values) const {
        glUniform4fv(location, values.size(), reinterpret_cast<const GLfloat*>(values.data()));
    }
    void Shader::set_uniform(const GLint location, const Vector<glm::dvec2>& values) const {
        glUniform2dv(location, values.size(), reinterpret_cast<const GLdouble*>(values.data()));
    }
    void Shader::set_uniform(const GLint location, const Vector<glm::dvec3>& values) const {
        glUniform3dv(location, values.size(), reinterpret_cast<const GLdouble*>(values.data()));
    }
    void Shader::set_uniform(const GLint location, const Vector<glm::dvec4>& values) const {
        glUniform4dv(location, values.size(), reinterpret_cast<const GLdouble*>(values.data()));
    }
    void Shader::set_uniform(const GLint location, const Vector<glm::mat4>& values) const {
        Vector<GLfloat> float_data(values.size() * 16);
        for (const auto& value : values) {
            const auto& p = glm::value_ptr(value);
            for (uint8_t i = 0; i < 16; ++i) {
                float_data.emplace_back(p[i]);
            }
        }

        glUniformMatrix4fv(location, values.size(), GL_FALSE, float_data.data());
    }

    bool Shader::get_uniform(const String& name, bool& value) const {
        const GLint loc = glGetUniformLocation(m_uProgram, name.as_cstr());
        if (loc < 0) {
            return false;
        }
        GLuint i;
        glGetUniformuiv(m_uProgram, loc, &i);
        value = i != 0;
        return true;
    }
    bool Shader::get_uniform(const String& name, GLint& value) const {
        const GLint loc = glGetUniformLocation(m_uProgram, name.as_cstr());
        if (loc < 0) {
            return false;
        }
        glGetUniformiv(m_uProgram, loc, &value);
        return true;
    }
    bool Shader::get_uniform(const String& name, GLuint& value) const {
        const GLint loc = glGetUniformLocation(m_uProgram, name.as_cstr());
        if (loc < 0) {
            return false;
        }
        glGetUniformuiv(m_uProgram, loc, &value);
        return true;
    }
    bool Shader::get_uniform(const String& name, GLfloat& value) const {
        const GLint loc = glGetUniformLocation(m_uProgram, name.as_cstr());
        if (loc < 0) {
            return false;
        }
        glGetUniformfv(m_uProgram, loc, &value);
        return true;
    }
    bool Shader::get_uniform(const String& name, GLdouble& value) const {
        const GLint loc = glGetUniformLocation(m_uProgram, name.as_cstr());
        if (loc < 0) {
            return false;
        }
        glGetUniformdv(m_uProgram, loc, &value);
        return true;
    }
    bool Shader::get_uniform(const String& name, glm::bvec2& value) const {
        const GLint loc = glGetUniformLocation(m_uProgram, name.as_cstr());
        if (loc < 0) {
            return false;
        }
        GLuint items[2] = { 0u };
        glGetUniformuiv(m_uProgram, loc, items);
        value = glm::bvec2(items[0] != 0, items[1] != 0);
        return true;
    }
    bool Shader::get_uniform(const String& name, glm::bvec3& value) const {
        const GLint loc = glGetUniformLocation(m_uProgram, name.as_cstr());
        if (loc < 0) {
            return false;
        }
        GLuint items[3] = { 0u };
        glGetUniformuiv(m_uProgram, loc, items);
        value = glm::bvec3(items[0] != 0, items[1] != 0, items[2] != 0);
        return true;
    }
    bool Shader::get_uniform(const String& name, glm::bvec4& value) const {
        const GLint loc = glGetUniformLocation(m_uProgram, name.as_cstr());
        if (loc < 0) {
            return false;
        }
        GLuint items[4] = { 0u };
        glGetUniformuiv(m_uProgram, loc, items);
        value = glm::bvec4(items[0] != 0, items[1] != 0, items[2] != 0, items[3] != 0);
        return true;
    }
    bool Shader::get_uniform(const String& name, glm::ivec2& value) const {
        const GLint loc = glGetUniformLocation(m_uProgram, name.as_cstr());
        if (loc < 0) {
            return false;
        }
        glGetUniformiv(m_uProgram, loc, &value[0]);
        return true;
    }
    bool Shader::get_uniform(const String& name, glm::ivec3& value) const {
        const GLint loc = glGetUniformLocation(m_uProgram, name.as_cstr());
        if (loc < 0) {
            return false;
        }
        glGetUniformiv(m_uProgram, loc, &value[0]);
        return true;
    }
    bool Shader::get_uniform(const String& name, glm::ivec4& value) const {
        const GLint loc = glGetUniformLocation(m_uProgram, name.as_cstr());
        if (loc < 0) {
            return false;
        }
        glGetUniformiv(m_uProgram, loc, &value[0]);
        return true;
    }
    bool Shader::get_uniform(const String& name, glm::uvec2& value) const {
        const GLint loc = glGetUniformLocation(m_uProgram, name.as_cstr());
        if (loc < 0) {
            return false;
        }
        glGetUniformuiv(m_uProgram, loc, &value[0]);
        return true;
    }
    bool Shader::get_uniform(const String& name, glm::uvec3& value) const {
        const GLint loc = glGetUniformLocation(m_uProgram, name.as_cstr());
        if (loc < 0) {
            return false;
        }
        glGetUniformuiv(m_uProgram, loc, &value[0]);
        return true;
    }
    bool Shader::get_uniform(const String& name, glm::uvec4& value) const {
        const GLint loc = glGetUniformLocation(m_uProgram, name.as_cstr());
        if (loc < 0) {
            return false;
        }
        glGetUniformuiv(m_uProgram, loc, &value[0]);
        return true;
    }
    bool Shader::get_uniform(const String& name, glm::vec2& value) const {
        const GLint loc = glGetUniformLocation(m_uProgram, name.as_cstr());
        if (loc < 0) {
            return false;
        }
        glGetUniformfv(m_uProgram, loc, &value[0]);
        return true;
    }
    bool Shader::get_uniform(const String& name, glm::vec3& value) const {
        const GLint loc = glGetUniformLocation(m_uProgram, name.as_cstr());
        if (loc < 0) {
            return false;
        }
        glGetUniformfv(m_uProgram, loc, &value[0]);
        return true;
    }
    bool Shader::get_uniform(const String& name, glm::vec4& value) const {
        const GLint loc = glGetUniformLocation(m_uProgram, name.as_cstr());
        if (loc < 0) {
            return false;
        }
        glGetUniformfv(m_uProgram, loc, &value[0]);
        return true;
    }
    bool Shader::get_uniform(const String& name, glm::dvec2& value) const {
        const GLint loc = glGetUniformLocation(m_uProgram, name.as_cstr());
        if (loc < 0) {
            return false;
        }
        glGetUniformdv(m_uProgram, loc, &value[0]);
        return true;
    }
    bool Shader::get_uniform(const String& name, glm::dvec3& value) const {
        const GLint loc = glGetUniformLocation(m_uProgram, name.as_cstr());
        if (loc < 0) {
            return false;
        }
        glGetUniformdv(m_uProgram, loc, &value[0]);
        return true;
    }
    bool Shader::get_uniform(const String& name, glm::dvec4& value) const {
        const GLint loc = glGetUniformLocation(m_uProgram, name.as_cstr());
        if (loc < 0) {
            return false;
        }
        return true;
    }
    bool Shader::get_uniform(const String& name, glm::mat4& value) const {
        const GLint loc = glGetUniformLocation(m_uProgram, name.as_cstr());
        if (loc < 0) {
            return false;
        }
        glGetUniformfv(m_uProgram, loc, &value[0][0]);
        return true;
    }

    void Shader::set_opaque(const bool value) {
        m_bOpaque = value;
    }
    void Shader::set_backface_culling(const bool value) {
        m_bBackfaceCulling = value;
    }

    GLint Shader::uniform_location(const String& name) const {
        return glGetUniformLocation(m_uProgram, name.as_cstr());
    }

    Result<ShaderUniformInfo> Shader::get_uniform_info(const String& name) const {
        const GLint loc = glGetUniformLocation(m_uProgram, name.as_cstr());
        if (loc < 0) {
            return Failure<ShaderUniformInfo>(std::format("No such uniform \"{}\"", name));
        }
        return get_uniform_info(loc);
    }

    Result<ShaderUniformInfo> Shader::get_uniform_info(const GLint location) const {
        if (location < 0) {
            return Failure<ShaderUniformInfo>(std::format("Uniform location {} is out of range", location));
        }

        String name(1024);
        GLenum type;
        glGetActiveUniform(m_uProgram, location, 1024, nullptr, nullptr, &type, name.data());
        name.recalculate_size();

        ShaderUniformInfo info;
        info.name = name;
        info.location = location;
        info.type = static_cast<ShaderUniformType>(type);
        return Success<ShaderUniformInfo>(info);
    }

    size_t Shader::get_uniform_count() const {
        GLint count = 0;
        glGetProgramiv(m_uProgram, GL_ACTIVE_UNIFORMS, &count);
        return count;
    }

    Result<ShaderPtr> Shader::Compile(const String& name, const String& vertex, const String& fragment) {
        GLint status;
        const GLuint vid = glCreateShader(GL_VERTEX_SHADER);
        if (vid == 0) {
            return Failure<ShaderPtr>(std::format("Failed to generate vertex shader: GL Error {}", glGetError()));
        }

        const auto processed_vertex = ResolveShaderIncludes(std::format("{}:vertex", name), vertex);
        if (!processed_vertex.has_value()) {
            return Failure<ShaderPtr>(std::format("Failed to compile vertex shader, error while preprocessing: {}", processed_vertex.error().message));
        }
        const char* vertex_cstr = processed_vertex.value().as_cstr();
        glShaderSource(vid, 1, &vertex_cstr, nullptr);
        glCompileShader(vid);
        glGetShaderiv(vid, GL_COMPILE_STATUS, &status);
        if (!status) {
            String info_log(2048);
            glGetShaderInfoLog(vid, 2048, nullptr, info_log.data());
            info_log.recalculate_size();
            glDeleteShader(vid);
            return Failure<ShaderPtr>(std::format("Failed to compile vertex shader: {}", info_log));
        }

        const GLuint fid = glCreateShader(GL_FRAGMENT_SHADER);
        if (fid == 0) {
            glDeleteShader(vid);
            return Failure<ShaderPtr>(std::format("Failed to generate fragment shader: GL Error {}", glGetError()));
        }

        const auto processed_fragment = ResolveShaderIncludes(std::format("{}:fragment", name), fragment);
        if (!processed_vertex.has_value()) {
            glDeleteShader(vid);
            return Failure<ShaderPtr>(std::format("Failed to compile fragment shader, error while preprocessing: {}", processed_fragment.error().message));
        }
        const char* fragment_cstr = processed_fragment.value().as_cstr();
        glShaderSource(fid, 1, &fragment_cstr, nullptr);
        glCompileShader(fid);
        glGetShaderiv(fid, GL_COMPILE_STATUS, &status);
        if (!status) {
            String info_log(2048);
            glGetShaderInfoLog(fid, 2048, nullptr, info_log.data());
            info_log.recalculate_size();
            glDeleteShader(fid);
            glDeleteShader(vid);
            return Failure<ShaderPtr>(std::format("Failed to compile fragment shader: {}", info_log));
        }

        const GLuint id = glCreateProgram();
        if (id == 0) {
            glDeleteShader(fid);
            glDeleteShader(vid);
            return Failure<ShaderPtr>(std::format("Failed to generate shader program: GL Error {}", glGetError()));
        }
        glAttachShader(id, vid);
        glAttachShader(id, fid);
        glLinkProgram(id);
        glGetShaderiv(id, GL_LINK_STATUS, &status);
        if (!status) {
            String info_log(2048);
            glGetProgramInfoLog(id, 2048, nullptr, info_log.data());
            info_log.recalculate_size();
            glDeleteShader(fid);
            glDeleteShader(vid);
            return Failure<ShaderPtr>(std::format("Failed to link shader program: {}", info_log));
        }
        glDeleteShader(fid);
        glDeleteShader(vid);

        return Success<ShaderPtr>(std::move(std::make_shared<Shader>(std::move(Shader { id }))));
    }
    Result<ShaderPtr> Shader::FromBinary(const String& name, const void* data, const size_t data_size,
        const String& vertex_entry, const String& fragment_entry) {
        GLint status;
        GLuint ids[] = { };
        ids[0] = glCreateShader(GL_VERTEX_SHADER);
        if (ids[0] == 0) {
            return Failure<ShaderPtr>(std::format("Failed to generate vertex shader: GL Error {}", glGetError()));
        }
        ids[1] = glCreateShader(GL_FRAGMENT_SHADER);
        if (ids[1] == 0) {
            return Failure<ShaderPtr>(std::format("Failed to generate vertex shader: GL Error {}", glGetError()));
        }

        glShaderBinary(2, ids, GL_SHADER_BINARY_FORMAT_SPIR_V, data, data_size);
        glSpecializeShader(ids[0], vertex_entry.as_cstr(), 0, nullptr, nullptr);
        glSpecializeShader(ids[1], fragment_entry.as_cstr(), 0, nullptr, nullptr);
        glGetShaderiv(ids[0], GL_COMPILE_STATUS, &status);
        if (!status) {
            String info_log(2048);
            glGetShaderInfoLog(ids[0], 2048, nullptr, info_log.data());
            info_log.recalculate_size();
            glDeleteShader(ids[0]);
            glDeleteShader(ids[1]);
            return Failure<ShaderPtr>(std::format("Failed to load vertex shader: {}", info_log));
        }
        glGetShaderiv(ids[1], GL_COMPILE_STATUS, &status);
        if (!status) {
            String info_log(2048);
            glGetShaderInfoLog(ids[1], 2048, nullptr, info_log.data());
            info_log.recalculate_size();
            glDeleteShader(ids[0]);
            glDeleteShader(ids[1]);
            return Failure<ShaderPtr>(std::format("Failed to load fragment shader: {}", info_log));
        }

        const GLuint id = glCreateProgram();
        if (id == 0) {
            glDeleteShader(ids[0]);
            glDeleteShader(ids[1]);
            return Failure<ShaderPtr>(std::format("Failed to generate shader program: GL Error {}", glGetError()));
        }
        glAttachShader(id, ids[0]);
        glAttachShader(id, ids[1]);
        glLinkProgram(id);
        glGetShaderiv(id, GL_LINK_STATUS, &status);
        if (!status) {
            String info_log(2048);
            glGetProgramInfoLog(id, 2048, nullptr, info_log.data());
            info_log.recalculate_size();
            glDeleteShader(ids[0]);
            glDeleteShader(ids[1]);
            return Failure<ShaderPtr>(std::format("Failed to link shader program: {}", info_log));
        }
        glDeleteShader(ids[0]);
        glDeleteShader(ids[1]);

        return Success<ShaderPtr>(std::make_shared<Shader>(std::move(Shader { id })));
    }
    Result<ShaderPtr> Shader::FromBinary(const String& name, const void* vertex_data, const size_t vertex_data_size,
        const void* fragment_data, const size_t fragment_data_size, const String& vertex_entry,
        const String& fragment_entry) {

        GLint status;
        const GLuint vid = glCreateShader(GL_VERTEX_SHADER);
        if (vid == 0) {
            return Failure<ShaderPtr>(std::format("Failed to generate vertex shader: GL Error {}", glGetError()));
        }
        glShaderBinary(1, &vid, GL_SHADER_BINARY_FORMAT_SPIR_V, vertex_data, vertex_data_size);
        glSpecializeShader(vid, vertex_entry.as_cstr(), 0, nullptr, nullptr);
        glGetShaderiv(vid, GL_COMPILE_STATUS, &status);
        if (!status) {
            String info_log(2048);
            glGetShaderInfoLog(vid, 2048, nullptr, info_log.data());
            info_log.recalculate_size();
            glDeleteShader(vid);
            return Failure<ShaderPtr>(std::format("Failed to load vertex shader: {}", info_log));
        }

        const GLuint fid = glCreateShader(GL_FRAGMENT_SHADER);
        if (fid == 0) {
            return Failure<ShaderPtr>(std::format("Failed to generate fragment shader: GL Error {}", glGetError()));
        }
        glShaderBinary(1, &fid, GL_SHADER_BINARY_FORMAT_SPIR_V, fragment_data, fragment_data_size);
        glSpecializeShader(fid, fragment_entry.as_cstr(), 0, nullptr, nullptr);
        glGetShaderiv(fid, GL_COMPILE_STATUS, &status);
        if (!status) {
            String info_log(2048);
            glGetShaderInfoLog(fid, 2048, nullptr, info_log.data());
            info_log.recalculate_size();
            glDeleteShader(fid);
            glDeleteShader(vid);
            return Failure<ShaderPtr>(std::format("Failed to load fragment shader: {}", info_log));
        }

        const GLuint id = glCreateProgram();
        if (id == 0) {
            glDeleteShader(fid);
            glDeleteShader(vid);
            return Failure<ShaderPtr>(std::format("Failed to generate shader program: GL Error {}", glGetError()));
        }
        glAttachShader(id, vid);
        glAttachShader(id, fid);
        glLinkProgram(id);
        glGetShaderiv(id, GL_LINK_STATUS, &status);
        if (!status) {
            String info_log(2048);
            glGetProgramInfoLog(id, 2048, nullptr, info_log.data());
            info_log.recalculate_size();
            glDeleteShader(fid);
            glDeleteShader(vid);
            return Failure<ShaderPtr>(std::format("Failed to link shader program: {}", info_log));
        }
        glDeleteShader(fid);
        glDeleteShader(vid);

        return Success<ShaderPtr>(std::make_shared<Shader>(std::move(Shader { id })));
    }

    Result<ShaderPtr> Shader::LoadAsset(const Path& path, const AssetLoaderFlags::Type flags) {
        if (path.extension().equals(".xml", StringCompareType::CaseInsensitive)) {
            const auto xml_str = Assets::LoadAsString(path, flags);
            if (xml_str.has_value()) {
                pugi::xml_document doc;
                if (const auto xml_result = doc.load_string(xml_str->as_cstr()); xml_result.status != pugi::status_ok) {
                    return Failure<ShaderPtr>(std::format("Failed to load shader \"{}\": Error while parsing XML '{}'", path, xml_result.description()));
                }

                const auto shader_node = doc.child("Shader");
                if (!shader_node) {
                    return Failure<ShaderPtr>(std::format("Failed to load shader \"{}\": Expected root node 'Shader' in XML document!", path));
                }

                Result<ShaderPtr> result = Failure<ShaderPtr>(std::format("Failed to load shader \"{}\": Expected child node 'Sources', 'Binaries' or 'Binary' in root node 'Shader'", path));
                if (const auto sources_node = shader_node.child("Sources"); sources_node) {
                    const auto vertex_attrib = sources_node.attribute("vertex");
                    const auto fragment_attrib = sources_node.attribute("fragment");
                    if (!vertex_attrib || !fragment_attrib) {
                        return Failure<ShaderPtr>(std::format("Failed to load shader \"{}\": Expected attributes 'vertex' and 'fragment' in Sources node!", path));
                    }

                    const auto vertex_str = Assets::LoadAsString(vertex_attrib.value());
                    if (!vertex_str.has_value()) {
                        return Failure<ShaderPtr>(std::format("Failed to load shader \"{}\": Failed to read vertex shader \"{}\"!", path, vertex_str.value()));
                    }
                    const auto fragment_str = Assets::LoadAsString(fragment_attrib.value());
                    if (!fragment_str.has_value()) {
                        return Failure<ShaderPtr>(std::format("Failed to load shader \"{}\": Failed to read fragment shader \"{}\"!", path, fragment_str.value()));
                    }
                    result = std::move(Compile(path.as_string(), vertex_str.value(), fragment_str.value()));
                    if (!result.has_value()) {
                        return result;
                    }
                }
                if (const auto binaries_node = shader_node.child("Binaries"); binaries_node) {
                    const auto vertex_node = binaries_node.child("Vertex");
                    const auto fragment_node = binaries_node.child("Fragment");
                    if (!vertex_node || !fragment_node) {
                        return Failure<ShaderPtr>(std::format("Failed to load shader \"{}\": Expected child nodes 'Vertex' and 'Fragment' in Binaries node!", path));
                    }

                    const auto vertex_data_attrib = vertex_node.attribute("data");
                    const auto vertex_entry_attrib = vertex_node.attribute("entry");
                    if (!vertex_data_attrib || !vertex_entry_attrib) {
                        return Failure<ShaderPtr>(std::format("Failed to load shader \"{}\": Expected attributes 'data' and 'entry' in Vertex node!", path));
                    }

                    const auto fragment_data_attrib = vertex_node.attribute("data");
                    const auto fragment_entry_attrib = vertex_node.attribute("entry");
                    if (!fragment_data_attrib || !fragment_entry_attrib) {
                        return Failure<ShaderPtr>(std::format("Failed to load shader \"{}\": Expected attributes 'data' and 'entry' in Fragment node!", path));
                    }

                    const auto vertex_data = Assets::LoadAsBytes(vertex_data_attrib.value());
                    if (!vertex_data.has_value()) {
                        return Failure<ShaderPtr>(std::format("Failed to load shader \"{}\": Failed to read vertex data \"{}\"!", path, vertex_data_attrib.value()));
                    }
                    const auto fragment_data = Assets::LoadAsBytes(fragment_data_attrib.value());
                    if (!fragment_data.has_value()) {
                        return Failure<ShaderPtr>(std::format("Failed to load shader \"{}\": Failed to read fragment data \"{}\"!", path, fragment_data_attrib.value()));
                    }
                    result = FromBinary(path.as_string(),
                        vertex_data->data(), vertex_data->size(),
                        fragment_data->data(), fragment_data->size(),
                        vertex_entry_attrib.value(), fragment_entry_attrib.value()
                    );
                    if (!result.has_value()) {
                        return result;
                    }
                }
                if (const auto binary_node = shader_node.child("Binary"); binary_node) {
                    const auto data_attrib = binary_node.attribute("data");
                    const auto vertex_entry_attrib = binary_node.attribute("vertex_entry");
                    const auto fragment_entry_attrib = binary_node.attribute("fragment_entry");
                    if (!data_attrib || !vertex_entry_attrib || !vertex_entry_attrib) {
                        return Failure<ShaderPtr>(std::format("Failed to load shader \"{}\": Expected attributes 'data', 'vertex_entry' and 'fragment_entry' in Binary node!", path));
                    }

                    const auto data = Assets::LoadAsBytes(data_attrib.value());
                    if (!data.has_value()) {
                        return Failure<ShaderPtr>(std::format("Failed to load shader \"{}\": Failed to read binary file \"{}\"", path, data_attrib.value()));
                    }
                    result = FromBinary(path.as_string(), data->data(), data->size(), vertex_entry_attrib.value(), fragment_entry_attrib.value());
                    if (!result.has_value()) {
                        return result;
                    }
                }
                if (!result.has_value()) {
                    return result;
                }

                for (const auto& node : shader_node.children()) {
                    if (String(node.name()).equals("BackfaceCulling")) {
                        const auto value_result = StringToBool(node.child_value());
                        if (!value_result.has_value()) {
                            return Failure<ShaderPtr>(std::format("Failed to load shader \"{}\": Parameter 'BackfaceCulling' must contain a boolean value!", path));
                        }
                        result.value()->set_backface_culling(value_result.value());
                    }
                    if (String(node.name()).equals("Opaque")) {
                        const auto value_result = StringToBool(node.child_value());
                        if (!value_result.has_value()) {
                            return Failure<ShaderPtr>(std::format("Failed to load shader \"{}\": Parameter 'Opaque' must contain a boolean value!", path));
                        }
                        result.value()->set_opaque(value_result.value());
                    }
                }

                return std::move(result);
            }
            return Failure<ShaderPtr>(std::format("Failed to load shader \"{}\": {}", path, xml_str.error().message));
        }
        return Failure<ShaderPtr>(std::format("Failed to load shader \"{}\": Expected asset extension '.xml'", path));
    }

    static ShaderPtr s_placeholder_shader = nullptr;

    ShaderPtr Shader::PlaceHolder() {
        if (s_placeholder_shader != nullptr) {
            return s_placeholder_shader;
        }
        const auto result = Compile(FOW_SHADER_PLACEHOLDER_ASSET_PATH,
                "#version 330 core\n"
                "layout (location = 0) in vec3 VERTEX_POSITION;\n"
                "uniform mat4 MATRIX_PVM;\n"
                "void main() {\n"
                "\tgl_Position = MATRIX_PVM * vec4(VERTEX_POSITION, 1.0);\n"
                "}",
                "#version 330 core\n"
                "out vec4 FRAGMENT_COLOR;\n"
                "void main() {\n"
                "\tFRAGMENT_COLOR = vec4(1.0, 0.0, 1.0, 1.0);\n"
                "}"
            );
        if (result.has_value()) {
            s_placeholder_shader = std::move(result.value());
            return s_placeholder_shader;
        }
        throw std::runtime_error(std::format("Failed to compile placeholder shader: {}", result.error().message));
    }

    void Shader::UnloadPlaceHolder() {
        if (s_placeholder_shader != nullptr) {
            s_placeholder_shader = nullptr;
        }
    }
}
