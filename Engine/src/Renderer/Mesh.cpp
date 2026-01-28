#include <glad/glad.h>
#include "fow/Renderer/Mesh.hpp"
#include "fow/Renderer.hpp"

namespace fow {
    Vector<Vertex> Vertex::CreateVertexArrayFromBuffers(const Vector<glm::vec3>& positions, const Vector<glm::vec3>& normals, const Vector<glm::vec3>& tangents, const Vector<glm::vec3>& bitangents, const Vector<glm::vec2>& uvs) {
        Vector<Vertex> vertices;
        vertices.reserve(positions.size());
        for (size_t i = 0; i < positions.size(); ++i) {
            vertices.emplace_back(
                positions.at(i),
                normals.at(i),
                tangents.at(i),
                bitangents.at(i),
                uvs.at(i)
            );
        }
        return vertices;
    }

    Mesh::~Mesh() {
        if (m_bInitialized) {
            if (m_uVao != 0) {
                glDeleteVertexArrays(1, &m_uVao);
            }
            if (m_uVbo != 0) {
                glDeleteBuffers(1, &m_uVbo);
            }
            if (m_uEbo != 0) {
                glDeleteBuffers(1, &m_uEbo);
            }
        }
    }

    Result<MeshPtr> Mesh::Create(const MaterialPtr& material, const std::vector<Vertex>& vertices, const std::vector<GLuint>& indices, MeshPrimitive primitive, MeshDrawMode draw_mode) {
        GLuint vao, vbo, ebo;
        glGenVertexArrays(1, &vao);
        if (vao == 0) {
            return Failure(std::format("Failed to generate vertex array handle: GL error {}", glGetError()));
        }
        glBindVertexArray(vao);

        glGenBuffers(1, &vbo);
        if (vbo == 0) {
            glDeleteVertexArrays(1, &vao);
            return Failure(std::format("Failed to generate vertex buffer object handle: GL error {}", glGetError()));
        }
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(vertices.size() * sizeof(Vertex)), vertices.data(), static_cast<GLenum>(draw_mode));

        glGenBuffers(1, &ebo);
        if (ebo == 0) {
            glDeleteVertexArrays(1, &vao);
            glDeleteBuffers(1, &vbo);
            return Failure(std::format("Failed to generate element buffer object handle: GL error {}", glGetError()));
        }
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(indices.size() * sizeof(GLuint)), indices.data(), static_cast<GLenum>(draw_mode));

        // Position
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(0));
        glEnableVertexAttribArray(0);
        // Normal
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        // Tangent
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(6 * sizeof(float)));
        glEnableVertexAttribArray(2);
        // Bitangent
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(9 * sizeof(float)));
        glEnableVertexAttribArray(3);
        // UV
        glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(12 * sizeof(float)));
        glEnableVertexAttribArray(4);

        glBindVertexArray(0);

        return Success<MeshPtr>(std::move(std::make_shared<Mesh>(std::move(Mesh { vao, vbo, ebo, static_cast<GLsizei>(indices.size()), material, primitive }))));
    }

    Result<MeshPtr> Mesh::CreateQuad(const MaterialPtr& material, const glm::vec2& scale, const MeshDrawMode draw_mode) {
        const Vector vertices = {
            Vertex { glm::vec3 { scale.x *  0.5f, scale.y * -0.5f, 0.0f },   glm::vec3{ 0.0f, 1.0f, 0.0f },   glm::vec3{ 0.0f, 1.0f, 0.0f },   glm::vec3{ 0.0f, 1.0f, 0.0f },   glm::vec2 { 1.0f, 1.0f } },
            Vertex { glm::vec3 { scale.x *  0.5f, scale.y *  0.5f, 0.0f },   glm::vec3{ 0.0f, 1.0f, 0.0f },   glm::vec3{ 0.0f, 1.0f, 0.0f },   glm::vec3{ 0.0f, 1.0f, 0.0f },   glm::vec2 { 1.0f, 0.0f } },
            Vertex { glm::vec3 { scale.x * -0.5f, scale.y *  0.5f, 0.0f },   glm::vec3{ 0.0f, 1.0f, 0.0f },   glm::vec3{ 0.0f, 1.0f, 0.0f },   glm::vec3{ 0.0f, 1.0f, 0.0f },   glm::vec2 { 0.0f, 0.0f } },
            Vertex { glm::vec3 { scale.x * -0.5f, scale.y * -0.5f, 0.0f },   glm::vec3{ 0.0f, 1.0f, 0.0f },   glm::vec3{ 0.0f, 1.0f, 0.0f },   glm::vec3{ 0.0f, 1.0f, 0.0f },   glm::vec2 { 0.0f, 1.0f } }
        };
        const Vector indices = {
            0u, 1u, 2u,
            0u, 2u, 3u
        };
        return Create(material, vertices, indices, MeshPrimitive::Triangles, draw_mode);
    }
    Result<MeshPtr> Mesh::CreateCube(const MaterialPtr& material, const glm::vec3& mins, const glm::vec3& maxs, const MeshDrawMode draw_mode) {
        return Failure("Not implemented");
    }
    Result<MeshPtr> Mesh::CreateCylinder(const MaterialPtr& material, float radius, float height, float subdivision, MeshDrawMode draw_mode) {
        return Failure("Not implemented");
    }
    Result<MeshPtr> Mesh::CreateCapsule(const MaterialPtr& material, float radius, float height, float subdivision, MeshDrawMode draw_mode) {
        return Failure("Not implemented");
    }
    Result<MeshPtr> Mesh::CreateSphere(const MaterialPtr& material, float radius, float subdivision, MeshDrawMode draw_mode) {
        return Failure("Not implemented");
    }

    const Mesh Mesh::Null = Mesh { };

    void Mesh::draw() const {
        if (m_pMaterial != nullptr && m_pMaterial->is_valid()) {
            Debug::Assert(m_pMaterial->apply());
            Debug::Assert(m_pMaterial->shader()->set_uniform("MATRIX_PROJECTION", Renderer::GetProjectionMatrix()), "Error while applying uniform \"MATRIX_PROJECTION\"");
            Debug::Assert(m_pMaterial->shader()->set_uniform("MATRIX_VIEW", Renderer::GetViewMatrix()), "Error while applying uniform \"MATRIX_VIEW\"");
            Debug::Assert(m_pMaterial->shader()->set_uniform("MATRIX_MODEL", glm::mat4 { 1.0f }), "Error while applying uniform \"MATRIX_MODEL\"");
        } else {
            Shader::PlaceHolder()->use();
            Debug::Assert(Shader::PlaceHolder()->set_uniform("MATRIX_PROJECTION", Renderer::GetProjectionMatrix()), "Error while applying uniform \"MATRIX_PROJECTION\"");
            Debug::Assert(Shader::PlaceHolder()->set_uniform("MATRIX_VIEW", Renderer::GetViewMatrix()), "Error while applying uniform \"MATRIX_VIEW\"");
            Debug::Assert(Shader::PlaceHolder()->set_uniform("MATRIX_MODEL", glm::mat4 { 1.0f }), "Error while applying uniform \"MATRIX_MODEL\"");
        }
        glBindVertexArray(m_uVao);
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(m_iIndexCount), GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0);
    }
    void Mesh::draw(const Transform& transform) const {
        if (m_pMaterial != nullptr && m_pMaterial->is_valid()) {
            Debug::Assert(m_pMaterial->apply());
            Debug::Assert(m_pMaterial->shader()->set_uniform("MATRIX_PROJECTION", Renderer::GetProjectionMatrix()), "Error while applying uniform \"MATRIX_PROJECTION\"");
            Debug::Assert(m_pMaterial->shader()->set_uniform("MATRIX_VIEW", Renderer::GetViewMatrix()), "Error while applying uniform \"MATRIX_VIEW\"");
            Debug::Assert(m_pMaterial->shader()->set_uniform("MATRIX_MODEL", transform.matrix()), "Error while applying uniform \"MATRIX_MODEL\"");
        } else {
            Shader::PlaceHolder()->use();
            Debug::Assert(Shader::PlaceHolder()->set_uniform("MATRIX_PROJECTION", Renderer::GetProjectionMatrix()), "Error while applying uniform \"MATRIX_PROJECTION\"");
            Debug::Assert(Shader::PlaceHolder()->set_uniform("MATRIX_VIEW", Renderer::GetViewMatrix()), "Error while applying uniform \"MATRIX_VIEW\"");
            Debug::Assert(Shader::PlaceHolder()->set_uniform("MATRIX_MODEL", transform.matrix()), "Error while applying uniform \"MATRIX_MODEL\"");
        }
        glBindVertexArray(m_uVao);
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(m_iIndexCount), GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0);
    }

    void MeshBuilder::append(const Vertex& vertex) {
        const auto it = std::ranges::find_if(m_vertices, [&vertex](const auto& v) {
            return v.position == vertex.position && v.normal == vertex.normal && v.uv == vertex.uv;
        });

        if (it != m_vertices.end()) {
            m_indices.emplace_back(std::distance(m_vertices.begin(), it));
        } else {
            m_indices.emplace_back(m_vertices.size());
            m_vertices.emplace_back(vertex);
        }
    }
    Result<MeshPtr> MeshBuilder::create_mesh(const MeshDrawMode draw_mode) const {
        return Mesh::Create(m_pMaterial, m_vertices, m_indices, m_ePrimitive, draw_mode);
    }
}
