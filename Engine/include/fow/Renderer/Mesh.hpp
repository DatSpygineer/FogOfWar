#ifndef FOW_RENDERER_MESH_HPP
#define FOW_RENDERER_MESH_HPP

#include <glm/glm.hpp>

#include "fow/Shared.hpp"
#include "fow/Renderer/Material.hpp"

namespace fow {
    struct FOW_RENDER_API Vertex {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 uv;

        Vertex(const Vertex& other) = default;
        Vertex(Vertex&& other) noexcept = default;
        Vertex(const glm::vec3& position, const glm::vec3& normal, const glm::vec2& uv) : position(position), normal(normal), uv(uv) { }

        Vertex& operator= (const Vertex& other) = default;
        Vertex& operator= (Vertex&& other) noexcept = default;

        static Vector<Vertex> CreateVertexArrayFromBuffers(const Vector<glm::vec3>& positions, const Vector<glm::vec3>& normals, const Vector<glm::vec2>& uvs);
    };

    enum class MeshDrawMode : GLenum {
        StaticDraw  = GL_STATIC_DRAW,
        StaticRead  = GL_STATIC_READ,
        StaticCopy  = GL_STATIC_COPY,
        DynamicDraw = GL_DYNAMIC_DRAW,
        DynamicRead = GL_DYNAMIC_READ,
        DynamicCopy = GL_DYNAMIC_COPY,
        StreamDraw  = GL_STREAM_DRAW,
        StreamRead  = GL_STREAM_READ,
        StreamCopy  = GL_STREAM_COPY
    };

    class Mesh;
    using MeshPtr = SharedPtr<Mesh>;

    enum class MeshPrimitive {
        Points        = GL_POINTS,
        Lines         = GL_LINES,
        LineStrip     = GL_LINE_STRIP,
        LineLoop      = GL_LINE_LOOP,
        Triangles     = GL_TRIANGLES,
        TriangleStrip = GL_TRIANGLE_STRIP,
        TriangleFan   = GL_TRIANGLE_FAN
    };

    class FOW_RENDER_API Mesh final {
        GLuint m_uVao, m_uVbo, m_uEbo;
        GLsizei m_iIndexCount;
        bool m_bInitialized;
        MaterialPtr m_pMaterial;
        MeshPrimitive m_ePrimitive;

        Mesh(const GLuint vao, const GLuint vbo, const GLuint ebo, const GLsizei index_count, const MaterialPtr& material, const MeshPrimitive primitive = MeshPrimitive::Triangles) :
            m_uVao( vao), m_uVbo(vbo), m_uEbo(ebo),
            m_iIndexCount(index_count),
            m_bInitialized(true), m_pMaterial(material),
            m_ePrimitive(primitive) { }

    public:
        Mesh() : m_uVao(0), m_uVbo(0), m_uEbo(0), m_iIndexCount(0), m_bInitialized(false), m_ePrimitive(MeshPrimitive::Triangles) { }
        Mesh(const Mesh& mesh) = delete;
        Mesh(Mesh&& mesh) noexcept :
            m_uVao(mesh.m_uVao), m_uVbo(mesh.m_uVbo), m_uEbo(mesh.m_uEbo),
            m_iIndexCount(mesh.m_iIndexCount),
            m_bInitialized(mesh.m_bInitialized), m_pMaterial(std::move(mesh.m_pMaterial)), m_ePrimitive(mesh.m_ePrimitive) {
            mesh.m_uVao = 0;
            mesh.m_uVbo = 0;
            mesh.m_uEbo = 0;
            mesh.m_iIndexCount = 0;
            mesh.m_bInitialized = false;
            mesh.m_pMaterial = std::make_shared<Material>();
        }
        ~Mesh();

        Mesh& operator=(const Mesh& mesh) = delete;
        Mesh& operator=(Mesh&& mesh) noexcept {
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

            m_uVao = mesh.m_uVao;
            m_uVbo = mesh.m_uVbo;
            m_uEbo = mesh.m_uEbo;
            m_iIndexCount = mesh.m_iIndexCount;
            m_bInitialized = mesh.m_bInitialized;
            m_pMaterial = mesh.m_pMaterial;
            m_ePrimitive = mesh.m_ePrimitive;

            mesh.m_uVao = 0;
            mesh.m_uVbo = 0;
            mesh.m_uEbo = 0;
            mesh.m_iIndexCount = 0;
            mesh.m_bInitialized = false;
            mesh.m_pMaterial = std::make_shared<Material>();

            return *this;
        }

        [[nodiscard]] FOW_CONSTEXPR GLuint vao() const { return m_uVao; }
        [[nodiscard]] FOW_CONSTEXPR GLuint vbo() const { return m_uVbo; }
        [[nodiscard]] FOW_CONSTEXPR GLuint ebo() const { return m_uEbo; }
        [[nodiscard]] FOW_CONSTEXPR GLsizei index_count() const { return m_iIndexCount; }
        [[nodiscard]] FOW_CONSTEXPR bool is_valid() const { return m_uVao != 0 && m_uVbo != 0 && m_uEbo != 0; }
        [[nodiscard]] FOW_CONSTEXPR MaterialPtr& material() { return m_pMaterial; }
        [[nodiscard]] FOW_CONSTEXPR const MaterialPtr& material() const { return m_pMaterial; }
        [[nodiscard]] FOW_CONSTEXPR MeshPrimitive primitive_type() const { return m_ePrimitive; }

        static Result<MeshPtr> Create(const MaterialPtr& material, const std::vector<Vertex>& vertices, const std::vector<GLuint>& indices, MeshPrimitive primitive = MeshPrimitive::Triangles, MeshDrawMode draw_mode = MeshDrawMode::StaticDraw);
        static Result<MeshPtr> CreateQuad(const MaterialPtr& material, const glm::vec2& scale = glm::vec2(1.0f), MeshDrawMode draw_mode = MeshDrawMode::StaticDraw);
        static Result<MeshPtr> CreateCube(const MaterialPtr& material, const glm::vec3& mins, const glm::vec3& maxs, MeshDrawMode draw_mode = MeshDrawMode::StaticDraw);
        static Result<MeshPtr> CreateCylinder(const MaterialPtr& material, float radius, float height, float subdivision, MeshDrawMode draw_mode = MeshDrawMode::StaticDraw);
        static Result<MeshPtr> CreateCapsule(const MaterialPtr& material, float radius, float height, float subdivision, MeshDrawMode draw_mode = MeshDrawMode::StaticDraw);
        static Result<MeshPtr> CreateSphere(const MaterialPtr& material, float radius, float subdivision, MeshDrawMode draw_mode = MeshDrawMode::StaticDraw);

        static const Mesh Null;

        void draw() const;
        void draw(const Transform& transform) const;
    };

    class FOW_RENDER_API MeshBuilder final {
        Vector<Vertex> m_vertices;
        Vector<GLuint> m_indices;
        MeshPrimitive m_ePrimitive;
        MaterialPtr m_pMaterial;
    public:
        MeshBuilder(const MeshPrimitive primitive, const MaterialPtr& material) : m_ePrimitive(primitive), m_pMaterial(material)         { }
        MeshBuilder(const MeshPrimitive primitive, MaterialPtr&& material)      : m_ePrimitive(primitive), m_pMaterial(std::move(material)) { }
        explicit MeshBuilder(const MeshPrimitive primitive)                  : m_ePrimitive(primitive), m_pMaterial(std::make_shared<Material>())   { }

        void append(const Vertex& vertex);
        inline void append(const glm::vec3& position, const glm::vec2& uv) {
            append(position, glm::vec3 { 0.0f, 1.0f, 0.0f }, uv);
        }
        inline void append(const glm::vec3& position, const glm::vec3& normal, const glm::vec2& uv) {
            append(Vertex { position, normal, uv });
        }

        Result<MeshPtr> create_mesh(MeshDrawMode draw_mode) const;
    };
}

#endif