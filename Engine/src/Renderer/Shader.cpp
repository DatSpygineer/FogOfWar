#include <glad/glad.h>
#include "fow/Renderer/Shader.hpp"

#include <glm/gtc/type_ptr.hpp>

#include "fow/Shared/StringConvertion.hpp"

namespace fow {
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

    GLint Shader::uniform_location(const String& name) const {
        return glGetUniformLocation(m_uProgram, name.as_cstr());
    }

    Result<ShaderUniformInfo> Shader::get_uniform_info(const String& name) const {
        const GLint loc = glGetUniformLocation(m_uProgram, name.as_cstr());
        if (loc < 0) {
            return Failure(std::format("No such uniform \"{}\"", name));
        }
        return get_uniform_info(loc);
    }

    Result<ShaderUniformInfo> Shader::get_uniform_info(const GLint location) const {
        if (location < 0) {
            return Failure(std::format("Uniform location {} is out of range", location));
        }

        if (!glIsProgram(m_uProgram)) {
            return Failure(std::format("Shader with id {} is not a valid Shader program!", m_uProgram));
        }

        GLint uniform_count;
        glGetProgramInterfaceiv(m_uProgram, GL_UNIFORM, GL_ACTIVE_RESOURCES, &uniform_count);


        constexpr GLenum properties[] = { GL_LOCATION, GL_TYPE, GL_NAME_LENGTH };
        for (size_t i = 0; i < uniform_count; i++) {
            GLint results[3];
            glGetProgramResourceiv(m_uProgram, GL_UNIFORM, i, 3, properties, 3, nullptr, results);
            if (results[0] == location) {
                String name(1024);
                glGetProgramResourceName(m_uProgram, GL_UNIFORM, i, name.capacity(), nullptr, name.data());
                name.recalculate_size();

                ShaderUniformInfo info;
                info.name = name;
                info.location = location;
                info.type = static_cast<ShaderUniformType>(results[1]);

                return Success<ShaderUniformInfo>(info);
            }
        }

        return Failure(std::format("Uniform at location {} not found!", location));
    }

    size_t Shader::get_uniform_count() const {
        GLint count = 0;
        glGetProgramiv(m_uProgram, GL_ACTIVE_UNIFORMS, &count);
        return count;
    }

    HashMap<String, ShaderUniformInfo> Shader::list_uniforms() const {
        HashMap<String, ShaderUniformInfo> uniforms;
        int count = 0;
        glGetProgramiv(m_uProgram, GL_ACTIVE_UNIFORMS, &count);
        uniforms.reserve(count);
        for (int i = 0; i < count; ++i) {
            if (const auto info = get_uniform_info(i); info.has_value()) {
                uniforms.emplace(info->name, info.value());
            }
        }
        return uniforms;
    }

    Result<ShaderPtr> Shader::Compile(const String& name, const String& vertex, const String& fragment) {
        if (IsCached(name)) {
            return FromCache(name);
        }

        GLint status;
        const GLuint vid = glCreateShader(GL_VERTEX_SHADER);
        if (vid == 0) {
            return Failure(std::format("Failed to generate vertex shader: GL Error {}", glGetError()));
        }

        const char* vertex_cstr = vertex.as_cstr();
        glShaderSource(vid, 1, &vertex_cstr, nullptr);
        glCompileShader(vid);
        glGetShaderiv(vid, GL_COMPILE_STATUS, &status);
        if (!status) {
            String info_log(2048);
            glGetShaderInfoLog(vid, 2048, nullptr, info_log.data());
            info_log.recalculate_size();
            glDeleteShader(vid);
            return Failure(std::format("Failed to compile vertex shader: {}", info_log));
        }

        const GLuint fid = glCreateShader(GL_FRAGMENT_SHADER);
        if (fid == 0) {
            glDeleteShader(vid);
            return Failure(std::format("Failed to generate fragment shader: GL Error {}", glGetError()));
        }

        const char* fragment_cstr = fragment.as_cstr();
        glShaderSource(fid, 1, &fragment_cstr, nullptr);
        glCompileShader(fid);
        glGetShaderiv(fid, GL_COMPILE_STATUS, &status);
        if (!status) {
            String info_log(2048);
            glGetShaderInfoLog(fid, 2048, nullptr, info_log.data());
            info_log.recalculate_size();
            glDeleteShader(fid);
            glDeleteShader(vid);
            return Failure(std::format("Failed to compile fragment shader: {}", info_log));
        }

        const GLuint id = glCreateProgram();
        if (id == 0) {
            glDeleteShader(fid);
            glDeleteShader(vid);
            return Failure(std::format("Failed to generate shader program: GL Error {}", glGetError()));
        }
        glAttachShader(id, vid);
        glAttachShader(id, fid);
        glLinkProgram(id);
        glGetProgramiv(id, GL_LINK_STATUS, &status);
        if (!status) {
            String info_log(2048);
            glGetProgramInfoLog(id, 2048, nullptr, info_log.data());
            info_log.recalculate_size();
            glDeleteShader(fid);
            glDeleteShader(vid);
            return Failure(std::format("Failed to link shader program: {}", info_log));
        }
        glDeleteShader(fid);
        glDeleteShader(vid);

        return Success<ShaderPtr>(CacheShader(std::move(std::make_shared<Shader>(std::move(Shader { name, id })))));
    }
    Result<ShaderPtr> Shader::FromBinary(const String& name, const void* data, const size_t data_size, const String& vertex_entry, const String& fragment_entry) {
        if (IsCached(name)) {
            return FromCache(name);
        }

        GLint status;
        GLuint ids[] = { };
        ids[0] = glCreateShader(GL_VERTEX_SHADER);
        if (ids[0] == 0) {
            return Failure(std::format("Failed to generate vertex shader: GL Error {}", glGetError()));
        }
        ids[1] = glCreateShader(GL_FRAGMENT_SHADER);
        if (ids[1] == 0) {
            return Failure(std::format("Failed to generate vertex shader: GL Error {}", glGetError()));
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
            return Failure(std::format("Failed to load vertex shader: {}", info_log));
        }
        glGetShaderiv(ids[1], GL_COMPILE_STATUS, &status);
        if (!status) {
            String info_log(2048);
            glGetShaderInfoLog(ids[1], 2048, nullptr, info_log.data());
            info_log.recalculate_size();
            glDeleteShader(ids[0]);
            glDeleteShader(ids[1]);
            return Failure(std::format("Failed to load fragment shader: {}", info_log));
        }

        const GLuint id = glCreateProgram();
        if (id == 0) {
            glDeleteShader(ids[0]);
            glDeleteShader(ids[1]);
            return Failure(std::format("Failed to generate shader program: GL Error {}", glGetError()));
        }
        glAttachShader(id, ids[0]);
        glAttachShader(id, ids[1]);
        glLinkProgram(id);
        glGetProgramiv(id, GL_LINK_STATUS, &status);
        if (!status) {
            String info_log(2048);
            glGetProgramInfoLog(id, 2048, nullptr, info_log.data());
            info_log.recalculate_size();
            glDeleteShader(ids[0]);
            glDeleteShader(ids[1]);
            return Failure(std::format("Failed to link shader program: {}", info_log));
        }
        glDeleteShader(ids[0]);
        glDeleteShader(ids[1]);

        return Success<ShaderPtr>(CacheShader(std::move(std::make_shared<Shader>(std::move(Shader { name, id })))));
    }
    Result<ShaderPtr> Shader::FromBinary(const String& name, const void* vertex_data, const size_t vertex_data_size,
        const void* fragment_data, const size_t fragment_data_size, const String& vertex_entry,
        const String& fragment_entry) {
        if (IsCached(name)) {
            return FromCache(name);
        }

        GLint status;
        const GLuint vid = glCreateShader(GL_VERTEX_SHADER);
        if (vid == 0) {
            return Failure(std::format("Failed to generate vertex shader: GL Error {}", glGetError()));
        }
        glShaderBinary(1, &vid, GL_SHADER_BINARY_FORMAT_SPIR_V, vertex_data, vertex_data_size);
        glSpecializeShader(vid, vertex_entry.as_cstr(), 0, nullptr, nullptr);
        glGetShaderiv(vid, GL_COMPILE_STATUS, &status);
        if (!status) {
            String info_log(2048);
            glGetShaderInfoLog(vid, 2048, nullptr, info_log.data());
            info_log.recalculate_size();
            glDeleteShader(vid);
            return Failure(std::format("Failed to load vertex shader: {}", info_log));
        }

        const GLuint fid = glCreateShader(GL_FRAGMENT_SHADER);
        if (fid == 0) {
            return Failure(std::format("Failed to generate fragment shader: GL Error {}", glGetError()));
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
            return Failure(std::format("Failed to load fragment shader: {}", info_log));
        }

        const GLuint id = glCreateProgram();
        if (id == 0) {
            glDeleteShader(fid);
            glDeleteShader(vid);
            return Failure(std::format("Failed to generate shader program: GL Error {}", glGetError()));
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
            return Failure(std::format("Failed to link shader program: {}", info_log));
        }
        glDeleteShader(fid);
        glDeleteShader(vid);

        return Success<ShaderPtr>(CacheShader(std::move(std::make_shared<Shader>(std::move(Shader { name, id })))));
    }

    static std::unordered_map<String, ShaderPtr> s_shaders;

    ShaderPtr Shader::PlaceHolder() {
        if (IsCached(FOW_SHADER_PLACEHOLDER_NAME)) {
            return FromCache(FOW_SHADER_PLACEHOLDER_NAME);
        }
        const auto result = Compile(FOW_SHADER_PLACEHOLDER_NAME,
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
            return CacheShader(std::move(result.value()));
        }
        throw std::runtime_error(std::format("Failed to compile placeholder shader: {}", result.error().message));
    }

    ShaderPtr Shader::CacheShader(const ShaderPtr& shader) {
        const auto name = shader->m_sName;
        if (!s_shaders.contains(name)) {
            s_shaders.emplace(name, shader);
        }
        return s_shaders.at(name);
    }
    ShaderPtr Shader::CacheShader(ShaderPtr&& shader) noexcept {
        const auto name = shader->m_sName;
        if (!s_shaders.contains(name)) {
            s_shaders.emplace(name, std::move(shader));
        }
        return s_shaders.at(name);
    }
    ShaderPtr Shader::FromCache(const String& name) {
        return s_shaders.contains(name) ? s_shaders.at(name) : nullptr;
    }
    bool Shader::IsCached(const String& name) {
        return s_shaders.contains(name);
    }

    void Shader::UnloadShaderCache() {
        s_shaders.clear();
    }
}
