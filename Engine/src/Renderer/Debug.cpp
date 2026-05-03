#include "fow/Renderer/Debug.hpp"

#include "fow/Renderer.hpp"
#include "fow/Renderer/Shader.hpp"

namespace fow::Debug {
    static PointMeshPtr s_pointMesh = nullptr;
    static WireMeshPtr s_debugMesh = nullptr;

    PointMesh::PointMesh(const Vector3& position, float size, const Color& color) : m_uVao(0), m_uVbo(0), m_fSize(size) {
        const auto vertex = WireMeshVertex { position, color };

        glGenVertexArrays(1, &m_uVao);
        glBindVertexArray(m_uVao);

        glGenBuffers(1, &m_uVbo);
        glBindBuffer(GL_ARRAY_BUFFER, m_uVbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(WireMeshVertex), &vertex, GL_DYNAMIC_DRAW);

        // Position
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(WireMeshVertex), reinterpret_cast<void*>(0));
        glEnableVertexAttribArray(0);

        // Color
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(WireMeshVertex), reinterpret_cast<void*>(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glBindVertexArray(0);
    }
    PointMesh::~PointMesh() {
        if (m_uVao != 0) {
            glDeleteVertexArrays(1, &m_uVao);
        }
        if (m_uVbo != 0) {
            glDeleteBuffers(1, &m_uVbo);
        }
    }

    void PointMesh::draw() const {
    }
    void PointMesh::draw(const Matrix4& model) const {
    }
    void PointMesh::draw(const Transform& transform) const {
    }

    WireMesh::WireMesh(const Vector<WireMeshVertex>& verts, const MeshPrimitive& primitive) : m_uVao(0), m_uVbo(0), m_iVertexCount(verts.size()), m_ePrimitive(primitive) {
        glGenVertexArrays(1, &m_uVao);
        glBindVertexArray(m_uVao);

        glGenBuffers(1, &m_uVbo);
        glBindBuffer(GL_ARRAY_BUFFER, m_uVbo);
        glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(WireMeshVertex), verts.data(), GL_DYNAMIC_DRAW);

        // Position
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(WireMeshVertex), reinterpret_cast<void*>(0));
        glEnableVertexAttribArray(0);

        // Color
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(WireMeshVertex), reinterpret_cast<void*>(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glBindVertexArray(0);
    }
    WireMesh::~WireMesh() {
        if (m_uVao != 0) {
            glDeleteVertexArrays(1, &m_uVao);
        }
        if (m_uVbo != 0) {
            glDeleteBuffers(1, &m_uVbo);
        }
    }

    void WireMesh::draw() const {
        draw(Matrix4Constants::Identity);
    }
    void WireMesh::draw(const Matrix4& model) const {
        auto shader = Shader::FromCache("DebugDraw");
        if (shader == nullptr) {
            const auto sources = ShaderLib::GetSourcesForShader("DebugDraw");
            if (!AssertFatal(sources)) {
                return;
            }
            const auto vertex_src = ShaderLib::GetSource(sources->vertex);
            if (!AssertFatal(vertex_src)) {
                return;
            }
            const auto fragment_src = ShaderLib::GetSource(sources->fragment);
            if (!AssertFatal(fragment_src)) {
                return;
            }

            auto result = Shader::Compile("DebugDraw", vertex_src.value(), fragment_src.value());
            if (!AssertFatal(result)) {
                return;
            }
            shader = result.value();
        }

        FOW_DISCARD(shader->use());
        FOW_DISCARD(shader->set_uniform("MATRIX_PROJECTION", Renderer::GetProjectionMatrix()));
        FOW_DISCARD(shader->set_uniform("MATRIX_VIEW", Renderer::GetViewMatrix()));
        FOW_DISCARD(shader->set_uniform("MATRIX_MODEL", model));

        glBindVertexArray(m_uVao);
        glDrawArrays(static_cast<GLenum>(m_ePrimitive), 0, m_iVertexCount);
        glBindVertexArray(0);
    }
    void WireMesh::draw(const Transform& transform) const {
        draw(transform.matrix());
    }

    void WireMesh::update_vertices(const Vector<WireMeshVertex>& verts, const MeshPrimitive& primitive) {
        glBindVertexArray(m_uVao);
        glBindBuffer(GL_ARRAY_BUFFER, m_uVbo);
        // Clear data
        glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(WireMeshVertex), nullptr, GL_DYNAMIC_DRAW);
        // Add new data
        glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(WireMeshVertex), verts.data(), GL_DYNAMIC_DRAW);
        m_iVertexCount = verts.size();
    }

    void FreeDebugMesh() {
        if (s_debugMesh != nullptr) {
            s_debugMesh = nullptr;
        }
        if (s_pointMesh != nullptr) {
            s_pointMesh = nullptr;
        }
    }

    void DrawPoint(const Vector3& position, float size, const Color& color) {
        if (s_debugMesh == nullptr) {
            s_debugMesh = CreateRef<WireMesh>(Vector<WireMeshVertex> {
                { position, color }
            }, MeshPrimitive::Points);
        } else {
            s_debugMesh->update_vertices(Vector<WireMeshVertex> {
                { position, color }
            }, MeshPrimitive::Points);
        }
        s_debugMesh->draw();
    }

    void DrawLine(const Vector3& start, const Vector3& end, const Color& color_start, const Color& color_end) {
        if (s_debugMesh == nullptr) {
            s_debugMesh = CreateRef<WireMesh>(Vector<WireMeshVertex> {
                { start, color_start },
                { end, color_end }
            }, MeshPrimitive::Lines);
        } else {
            s_debugMesh->update_vertices(Vector<WireMeshVertex> {
                { start, color_start },
                { end, color_end }
            }, MeshPrimitive::Lines);
        }
        s_debugMesh->draw();
    }

    void DrawAxis(const Vector3& origin, const Quat& rotation, const Vector3& x_axis, const Vector3& y_axis, const Vector3& z_axis, const float size) {
        const auto transform = Transform {
            origin,
            Vector3Constants::One,
            rotation,
            nullptr
        };

        const auto verts = Vector {
            WireMeshVertex { Vector3Constants::Zero, ColorConstants::Red   },
            WireMeshVertex { x_axis * size,          ColorConstants::Red   },
            WireMeshVertex { Vector3Constants::Zero, ColorConstants::Green },
            WireMeshVertex { y_axis * size,          ColorConstants::Green },
            WireMeshVertex { Vector3Constants::Zero, ColorConstants::Blue  },
            WireMeshVertex { z_axis * size,          ColorConstants::Blue  }
        };

        if (s_debugMesh == nullptr) {
            s_debugMesh = CreateRef<WireMesh>(verts, MeshPrimitive::Lines);
        } else {
            s_debugMesh->update_vertices(verts, MeshPrimitive::Lines);
        }
        s_debugMesh->draw(transform);
    }

    void DrawQuad(const Vector3& origin, const Quat& rotation, const Vector2& size, const Color& color) {
        const auto transform = Transform {
            origin,
            Vector3Constants::One,
            rotation,
            nullptr
        };

        const auto verts = Vector {
            WireMeshVertex { Vector3Constants::Zero, color },
            WireMeshVertex { Vector3Constants::Zero + Vector3Constants::UnitY * size.y, color },
            WireMeshVertex { Vector3Constants::Zero + Vector3Constants::UnitX * size.x + Vector3Constants::UnitY * size.y, color },
            WireMeshVertex { Vector3Constants::Zero + Vector3Constants::UnitX * size.x, color },
        };

        if (s_debugMesh == nullptr) {
            s_debugMesh = CreateRef<WireMesh>(verts, MeshPrimitive::LineLoop);
        } else {
            s_debugMesh->update_vertices(verts, MeshPrimitive::LineLoop);
        }

        s_debugMesh->draw(transform);
    }

    void DrawCube(const Vector3& origin, const Quat& rotation, const Vector3& mins, const Vector3& maxs, const Color& color) {
        const auto transform = Transform {
            origin,
            Vector3Constants::One,
            rotation,
            nullptr
        };

        const auto min_x = mins.x;
        const auto max_x = maxs.x;
        const auto min_y = mins.y;
        const auto max_y = maxs.y;
        const auto min_z = mins.z;
        const auto max_z = maxs.z;

        const auto verts = Vector {
            // Front face
            WireMeshVertex { mins, color },
            WireMeshVertex { Vector3(min_x, max_y, min_z), color },
            WireMeshVertex { Vector3(min_x, max_y, min_z), color },
            WireMeshVertex { Vector3(max_x, max_y, min_z), color },
            WireMeshVertex { Vector3(max_x, max_y, min_z), color },
            WireMeshVertex { Vector3(max_x, min_y, min_z), color },
            WireMeshVertex { Vector3(max_x, min_y, min_z), color },
            WireMeshVertex { mins, color },
            // Back face
            WireMeshVertex { Vector3(min_x, min_y, max_z), color },
            WireMeshVertex { Vector3(min_x, max_y, max_z), color },
            WireMeshVertex { Vector3(min_x, max_y, max_z), color },
            WireMeshVertex { maxs, color },
            WireMeshVertex { maxs, color },
            WireMeshVertex { Vector3(max_x, min_y, max_z), color },
            WireMeshVertex { Vector3(max_x, min_y, max_z), color },
            WireMeshVertex { Vector3(min_x, min_y, max_z), color },
            // Bottom face
            WireMeshVertex { mins, color },
            WireMeshVertex { Vector3(min_x, min_y, max_z), color },
            WireMeshVertex { Vector3(max_x, min_y, min_z), color },
            WireMeshVertex { Vector3(max_x, min_y, max_z), color },
            // Top face
            WireMeshVertex { Vector3(min_x, max_y, min_z), color },
            WireMeshVertex { Vector3(min_x, max_y, max_z), color },
            WireMeshVertex { Vector3(max_x, max_y, min_z), color },
            WireMeshVertex { Vector3(max_x, max_y, max_z), color },
        };

        if (s_debugMesh == nullptr) {
            s_debugMesh = CreateRef<WireMesh>(verts, MeshPrimitive::Lines);
        } else {
            s_debugMesh->update_vertices(verts, MeshPrimitive::Lines);
        }
        s_debugMesh->draw(transform);
    }

    void DrawCross(const Vector3& origin, const Quat& rotation, float size, const Color& color) {
        const auto transform = Transform {
            origin,
            Vector3Constants::One,
            rotation,
            nullptr
        };

        const auto verts = Vector {
            WireMeshVertex { origin + (Vector3Constants::Left + Vector3Constants::Up) * size, color },
            WireMeshVertex { origin + (Vector3Constants::Right + Vector3Constants::Down) * size, color },
            WireMeshVertex { origin + (Vector3Constants::Left + Vector3Constants::Down) * size, color },
            WireMeshVertex { origin + (Vector3Constants::Right + Vector3Constants::Up) * size, color },
        };

        if (s_debugMesh == nullptr) {
            s_debugMesh = CreateRef<WireMesh>(verts, MeshPrimitive::Lines);
        } else {
            s_debugMesh->update_vertices(verts, MeshPrimitive::Lines);
        }
        s_debugMesh->draw(transform);
    }

    void DrawArrow(const Vector3& start, const Vector3& end, const float head_size, const Color& start_color, const Color& end_color) {
        const auto forward = glm::normalize(end - start);
        const auto right   = glm::cross(forward,
            forward == Vector3Constants::Up
                ? Vector3Constants::Right
                : forward == Vector3Constants::Down
                      ? Vector3Constants::Left
                      : Vector3Constants::Up
        );

        const auto verts = Vector {
            // Tail
            WireMeshVertex { start, start_color },
            WireMeshVertex { end, end_color },
            // Head
            WireMeshVertex { end, end_color },
            WireMeshVertex { end - forward * 0.5f * head_size + right * 0.5f * head_size, end_color },
            WireMeshVertex { end, end_color },
            WireMeshVertex { end - forward  * 0.5f * head_size - right * 0.5f * head_size, end_color },
        };

        if (s_debugMesh == nullptr) {
            s_debugMesh = CreateRef<WireMesh>(verts, MeshPrimitive::Lines);
        } else {
            s_debugMesh->update_vertices(verts, MeshPrimitive::Lines);
        }
        s_debugMesh->draw();
    }
}
