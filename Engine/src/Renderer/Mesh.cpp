#include "fow/Renderer/GL.hpp"
#include "fow/Renderer/Mesh.hpp"
#include "fow/Renderer.hpp"

namespace fow {
    Vector<Vertex> Vertex::CreateVertexArrayFromBuffers(const Vector<Vector3>& positions, const Vector<Vector3>& normals, const Vector<Vector3>& tangents, const Vector<Vector3>& bitangents, const Vector<Vector2>& uvs) {
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

    Vector<Vertex2D> Vertex2D::CreateVertexArrayFromBuffers(const Vector<Vector2>& positions, const Vector<Vector2>& uvs) {
        Vector<Vertex2D> vertices;
        vertices.reserve(positions.size());
        for (size_t i = 0; i < positions.size(); ++i) {
            vertices.emplace_back(positions.at(i), uvs.at(i));
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

    void Mesh::update_data(const Vector<Vertex>& vertices, const Vector<GLuint>& indices) {

    }
    void Mesh::update_data_2d(const Vector<Vertex2D>& vertices, const Vector<GLuint>& indices) {

    }

    void Mesh::set_material(const MaterialPtr& material) {
        m_pMaterial = material;
    }

    Result<MeshPtr> Mesh::Create(const MaterialPtr& material, const std::vector<Vertex>& vertices, const std::vector<GLuint>& indices, const MeshPrimitive primitive, MeshDrawMode draw_mode) {
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

    Result<MeshPtr> Mesh::Create2D(const MaterialPtr& material, const std::vector<Vertex2D>& vertices, const std::vector<GLuint>& indices, const MeshPrimitive primitive, MeshDrawMode draw_mode) {
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
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2D), reinterpret_cast<void*>(0));
        glEnableVertexAttribArray(0);
        // UV
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2D), reinterpret_cast<void*>(2 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glBindVertexArray(0);

        return Success<MeshPtr>(std::move(std::make_shared<Mesh>(std::move(Mesh { vao, vbo, ebo, static_cast<GLsizei>(indices.size()), material, primitive }))));
    }

    Result<MeshPtr> Mesh::CreateQuad(const MaterialPtr& material, const Vector2& scale, const MeshDrawMode draw_mode) {
        const Vector vertices = {
            Vertex { Vector3 { scale.x *  0.5f, scale.y * -0.5f, 0.0f },   Vector3{ 0.0f, 1.0f, 0.0f },   Vector3{ 0.0f, 1.0f, 0.0f },   Vector3{ 0.0f, 1.0f, 0.0f },   Vector2 { 1.0f, 1.0f } },
            Vertex { Vector3 { scale.x *  0.5f, scale.y *  0.5f, 0.0f },   Vector3{ 0.0f, 1.0f, 0.0f },   Vector3{ 0.0f, 1.0f, 0.0f },   Vector3{ 0.0f, 1.0f, 0.0f },   Vector2 { 1.0f, 0.0f } },
            Vertex { Vector3 { scale.x * -0.5f, scale.y *  0.5f, 0.0f },   Vector3{ 0.0f, 1.0f, 0.0f },   Vector3{ 0.0f, 1.0f, 0.0f },   Vector3{ 0.0f, 1.0f, 0.0f },   Vector2 { 0.0f, 0.0f } },
            Vertex { Vector3 { scale.x * -0.5f, scale.y * -0.5f, 0.0f },   Vector3{ 0.0f, 1.0f, 0.0f },   Vector3{ 0.0f, 1.0f, 0.0f },   Vector3{ 0.0f, 1.0f, 0.0f },   Vector2 { 0.0f, 1.0f } }
        };
        const Vector indices = {
            0u, 1u, 2u,
            0u, 2u, 3u
        };
        return Create(material, vertices, indices, MeshPrimitive::Triangles, draw_mode);
    }
    Result<MeshPtr> Mesh::CreateCube(const MaterialPtr& material, const Vector3& mins, const Vector3& maxs, const MeshDrawMode draw_mode) {
        return Failure("Not implemented");
    }
    Result<MeshPtr> Mesh::CreateCylinder(const MaterialPtr& material, float radius, float height, const uint32_t segments, MeshDrawMode draw_mode) {
        return Failure("Not implemented");
    }
    Result<MeshPtr> Mesh::CreateCapsule(const MaterialPtr& material, float radius, float height, const uint32_t segments, MeshDrawMode draw_mode) {
        return Failure("Not implemented");
    }
    Result<MeshPtr> Mesh::CreateSphere(const MaterialPtr& material, const float radius, const uint32_t segments, const MeshDrawMode draw_mode) {
        return Failure("Not implemented");
    }

    Result<MeshPtr> Mesh::CreateQuad2D(const MaterialPtr& material, const MeshDrawMode draw_mode) {
        return Create2D(
            material,
            {
                Vertex2D { Vector2 { 0.0f, 0.0f }, Vector2 { 0.0f, 0.0f } },
                Vertex2D { Vector2 { 0.0f, 1.0f }, Vector2 { 0.0f, 1.0f } },
                Vertex2D { Vector2 { 1.0f, 1.0f }, Vector2 { 1.0f, 1.0f } },
                Vertex2D { Vector2 { 1.0f, 0.0f }, Vector2 { 1.0f, 0.0f } },
            },
            {
                0u, 1u, 2u,
                2u, 3u, 0u
            },
            MeshPrimitive::Triangles,
            draw_mode
        );
    }

    const Mesh Mesh::Null = Mesh { };

    static void MeshDraw(const GLuint vao, const GLsizei index_count, const MaterialPtr& material, const Matrix4& model_matrix) {
        if (material != nullptr && material->is_valid()) {
            Debug::Assert(material->apply());
            Debug::Assert(material->shader()->set_uniform("MATRIX_PROJECTION", Renderer::GetProjectionMatrix()), "Error while applying uniform \"MATRIX_PROJECTION\"");
            Debug::Assert(material->shader()->set_uniform("MATRIX_VIEW", Renderer::GetViewMatrix()), "Error while applying uniform \"MATRIX_VIEW\"");
            Debug::Assert(material->shader()->set_uniform("MATRIX_MODEL[0]", model_matrix), "Error while applying uniform \"MATRIX_MODEL\"");
        } else {
            Shader::PlaceHolder()->use();
            Debug::Assert(Shader::PlaceHolder()->set_uniform("MATRIX_PROJECTION", Renderer::GetProjectionMatrix()), "Error while applying uniform \"MATRIX_PROJECTION\"");
            Debug::Assert(Shader::PlaceHolder()->set_uniform("MATRIX_VIEW", Renderer::GetViewMatrix()), "Error while applying uniform \"MATRIX_VIEW\"");
            Debug::Assert(Shader::PlaceHolder()->set_uniform("MATRIX_MODEL[0]", model_matrix), "Error while applying uniform \"MATRIX_MODEL\"");
        }
        RenderQueue::ApplyCurrentSceneParamsToMaterial(material);
        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, index_count, GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0);
    }
    static void MeshDrawInstances(const GLuint vao, const GLsizei index_count, const MaterialPtr& material, const Vector<Transform>& transforms) {
        if (material != nullptr && material->is_valid()) {
            Debug::Assert(material->apply());
            Debug::Assert(material->shader()->set_uniform("MATRIX_PROJECTION", Renderer::GetProjectionMatrix()), "Error while applying uniform \"MATRIX_PROJECTION\"");
            Debug::Assert(material->shader()->set_uniform("MATRIX_VIEW", Renderer::GetViewMatrix()), "Error while applying uniform \"MATRIX_VIEW\"");
            auto i = 0;
            for (const auto& transform : transforms) {
                Debug::Assert(material->shader()->set_uniform(
                    std::format("MATRIX_MODEL[{}]", i),
                    transform.matrix()),
                    std::format("Error while applying uniform \"MATRIX_MODEL[{}]\"", i)
                );
                ++i;
            }
        } else {
            Shader::PlaceHolder()->use();
            Debug::Assert(Shader::PlaceHolder()->set_uniform("MATRIX_PROJECTION", Renderer::GetProjectionMatrix()), "Error while applying uniform \"MATRIX_PROJECTION\"");
            Debug::Assert(Shader::PlaceHolder()->set_uniform("MATRIX_VIEW", Renderer::GetViewMatrix()), "Error while applying uniform \"MATRIX_VIEW\"");
            auto i = 0;
            for (const auto& transform : transforms) {
                Debug::Assert(Shader::PlaceHolder()->set_uniform(
                    std::format("MATRIX_MODEL[{}]", i),
                    transform.matrix()),
                    std::format("Error while applying uniform \"MATRIX_MODEL[{}]\"", i)
                );
                ++i;
            }
        }

        glBindVertexArray(vao);
        glDrawElementsInstanced(GL_TRIANGLES, static_cast<GLsizei>(index_count), GL_UNSIGNED_INT, nullptr, static_cast<GLsizei>(transforms.size()));
        glBindVertexArray(0);
    }

    static void MeshDraw2D(const GLuint vao, const GLsizei index_count, const MaterialPtr& material, const Rectangle& rect) {
        if (material != nullptr && material->is_valid()) {
            Debug::Assert(material->apply());
            Debug::Assert(material->shader()->set_uniform("MATRIX_PROJECTION", Renderer::GetProjectionMatrix2D()), "Error while applying uniform \"MATRIX_PROJECTION\"");
            Debug::Assert(material->shader()->set_uniform("AreaPosition", rect.position()), "Error while applying uniform \"AreaPosition\"");
            Debug::Assert(material->shader()->set_uniform("AreaSize", rect.size()), "Error while applying uniform \"AreaSize\"");
        } else {
            Shader::PlaceHolder()->use();
            Debug::Assert(Shader::PlaceHolder()->set_uniform("MATRIX_PROJECTION", Renderer::GetProjectionMatrix2D()), "Error while applying uniform \"MATRIX_PROJECTION\"");
            Debug::Assert(Shader::PlaceHolder()->set_uniform("AreaPosition", rect.position()), "Error while applying uniform \"AreaPosition\"");
            Debug::Assert(Shader::PlaceHolder()->set_uniform("AreaSize", rect.size()), "Error while applying uniform \"AreaSize\"");
        }
        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, index_count, GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0);
    }

    void Mesh::draw() const {
        draw(Matrix4Constants::Identity);
    }
    void Mesh::draw(const Transform& transform) const {
        draw(transform.matrix());
    }

    void Mesh::draw(const Matrix4& model_matrix) const {
        MeshDraw(m_uVao, m_iIndexCount, m_pMaterial, model_matrix);
    }
    void Mesh::draw(const MaterialPtr& override_material) const {
        draw(override_material, Matrix4Constants::Identity);
    }
    void Mesh::draw(const MaterialPtr& override_material, const Transform& transform) const {
        draw(override_material, transform.matrix());
    }
    void Mesh::draw(const MaterialPtr& override_material, const Matrix4& model_matrix) const {
        MeshDraw(m_uVao, m_iIndexCount, override_material, model_matrix);
    }

    void Mesh::draw_instances(const Vector<Transform>& transforms) const {
        MeshDrawInstances(m_uVao, m_iIndexCount, m_pMaterial, transforms);
    }
    void Mesh::draw_instances(const MaterialPtr& override_material, const Vector<Transform>& transforms) const {
        MeshDrawInstances(m_uVao, m_iIndexCount, override_material, transforms);
    }

    void Mesh::draw_2d(const Rectangle& rect) const {
        MeshDraw2D(m_uVao, m_iIndexCount, m_pMaterial, rect);
    }
    void Mesh::draw_2d(const Rectangle& rect, const MaterialPtr& override_material) const {
        MeshDraw2D(m_uVao, m_iIndexCount, override_material, rect);
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
