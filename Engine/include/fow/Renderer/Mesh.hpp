#ifndef FOW_RENDERER_MESH_HPP
#define FOW_RENDERER_MESH_HPP

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "fow/Shared.hpp"
#include "fow/String.hpp"
#include "fow/Assets.hpp"
#include "fow/Transform.hpp"
#include "fow/Renderer/Material.hpp"

namespace fow {
#ifdef _MSC_VER
    #pragma pack(push, 1)
    struct FOW_RENDER_API Vertex {
#else
    struct FOW_RENDER_API __attribute__((__packed__)) Vertex {
#endif
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
#ifdef _MSC_VER
    #pragma pack(pop)
#endif

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
        Material m_Material;
        MeshPrimitive m_ePrimitive;

        Mesh(const GLuint vao, const GLuint vbo, const GLuint ebo, const GLsizei index_count, const Material& material, const MeshPrimitive primitive = MeshPrimitive::Triangles) :
            m_uVao( vao), m_uVbo(vbo), m_uEbo(ebo),
            m_iIndexCount(index_count),
            m_bInitialized(true), m_Material(material),
            m_ePrimitive(primitive) { }

    public:
        Mesh() : m_uVao(0), m_uVbo(0), m_uEbo(0), m_iIndexCount(0), m_bInitialized(false), m_ePrimitive(MeshPrimitive::Triangles) { }
        Mesh(const Mesh& mesh) = delete;
        Mesh(Mesh&& mesh) noexcept :
            m_uVao(mesh.m_uVao), m_uVbo(mesh.m_uVbo), m_uEbo(mesh.m_uEbo),
            m_iIndexCount(mesh.m_iIndexCount),
            m_bInitialized(mesh.m_bInitialized), m_Material(std::move(mesh.m_Material)), m_ePrimitive(mesh.m_ePrimitive) {
            mesh.m_uVao = 0;
            mesh.m_uVbo = 0;
            mesh.m_uEbo = 0;
            mesh.m_iIndexCount = 0;
            mesh.m_bInitialized = false;
            mesh.m_Material = Material::Null;
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
            m_Material = mesh.m_Material;
            m_ePrimitive = mesh.m_ePrimitive;

            mesh.m_uVao = 0;
            mesh.m_uVbo = 0;
            mesh.m_uEbo = 0;
            mesh.m_iIndexCount = 0;
            mesh.m_bInitialized = false;
            mesh.m_Material = Material::Null;

            return *this;
        }

        [[nodiscard]] constexpr GLuint vao() const { return m_uVao; }
        [[nodiscard]] constexpr GLuint vbo() const { return m_uVbo; }
        [[nodiscard]] constexpr GLuint ebo() const { return m_uEbo; }
        [[nodiscard]] constexpr GLsizei index_count() const { return m_iIndexCount; }
        [[nodiscard]] constexpr bool is_valid() const { return m_uVao != 0 && m_uVbo != 0 && m_uEbo != 0; }
        [[nodiscard]] constexpr Material& material() { return m_Material; }
        [[nodiscard]] constexpr const Material& material() const { return m_Material; }
        [[nodiscard]] constexpr MeshPrimitive primitive_type() const { return m_ePrimitive; }

        static Result<MeshPtr> Create(const Material& material, const std::vector<Vertex>& vertices, const std::vector<GLuint>& indices, MeshPrimitive primitive = MeshPrimitive::Triangles, MeshDrawMode draw_mode = MeshDrawMode::StaticDraw);
        static Result<MeshPtr> CreateQuad(const Material& material, const glm::vec2& scale = glm::vec2(1.0f), MeshDrawMode draw_mode = MeshDrawMode::StaticDraw);
        static Result<MeshPtr> CreateCube(const Material& material, const glm::vec3& mins, const glm::vec3& maxs, MeshDrawMode draw_mode = MeshDrawMode::StaticDraw);
        static Result<MeshPtr> CreateCylinder(const Material& material, float radius, float height, float subdivision, MeshDrawMode draw_mode = MeshDrawMode::StaticDraw);
        static Result<MeshPtr> CreateCapsule(const Material& material, float radius, float height, float subdivision, MeshDrawMode draw_mode = MeshDrawMode::StaticDraw);
        static Result<MeshPtr> CreateSphere(const Material& material, float radius, float subdivision, MeshDrawMode draw_mode = MeshDrawMode::StaticDraw);

        static const Mesh Null;

        void draw() const;
        void draw(const Transform& transform) const;
    };

    class FOW_RENDER_API MeshBuilder final {
        Vector<Vertex> m_vertices;
        Vector<GLuint> m_indices;
        MeshPrimitive m_ePrimitive;
        Material m_material;
    public:
        MeshBuilder(const MeshPrimitive primitive, const Material& material) : m_ePrimitive(primitive), m_material(material)         { }
        MeshBuilder(const MeshPrimitive primitive, Material&& material)      : m_ePrimitive(primitive), m_material(std::move(material)) { }
        explicit MeshBuilder(const MeshPrimitive primitive)                  : m_ePrimitive(primitive), m_material(Material::Null)   { }

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