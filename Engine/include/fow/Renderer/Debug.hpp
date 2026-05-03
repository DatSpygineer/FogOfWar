#ifndef FOW_RENDERER_DEBUG_HPP
#define FOW_RENDERER_DEBUG_HPP

#include <fow/Shared.hpp>
#include <fow/Renderer/GL.hpp>

#include "Mesh.hpp"
#include "RenderShared.hpp"

namespace fow::Debug {
    struct FOW_RENDER_API WireMeshVertex {
        Vector3 position;
        Color color;
    };

    class FOW_RENDER_API PointMesh : public IDrawable3D {
        GLuint m_uVao, m_uVbo;
        float m_fSize;
    public:
        PointMesh(const Vector3& position, float size, const Color& color);
        ~PointMesh() override;

        void draw() const;
        void draw(const Matrix4& model) const;
        void draw(const Transform& transform) const override;
    };
    using PointMeshPtr = Ref<PointMesh>;

    class FOW_RENDER_API WireMesh : public IDrawable3D {
        GLuint m_uVao, m_uVbo;
        GLsizei m_iVertexCount;
        MeshPrimitive m_ePrimitive;
    public:
        WireMesh(const Vector<WireMeshVertex>& verts, const MeshPrimitive& primitive);
        ~WireMesh() override;

        void draw() const;
        void draw(const Matrix4& model) const;
        void draw(const Transform& transform) const override;

        void update_vertices(const Vector<WireMeshVertex>& verts, const MeshPrimitive& primitive);
    };
    using WireMeshPtr = Ref<WireMesh>;

    void FreeDebugMesh();

    void DrawPoint(const Vector3& position, float size, const Color& color);
    void DrawLine(const Vector3& start, const Vector3& end, const Color& color_start, const Color& color_end);
    inline void DrawLine(const Vector3& start, const Vector3& end, const Color& color) { DrawLine(start, end, color, color); }
    void DrawAxis(const Vector3& origin, const Quat& rotation, const Vector3& x_axis, const Vector3& y_axis, const Vector3& z_axis, float size = 1.0f);
    inline void DrawAxis(const Vector3& origin, const Vector3& x_axis, const Vector3& y_axis, const Vector3& z_axis, const float size = 1.0f) {
        DrawAxis(origin, QuatConstants::Identity, x_axis, y_axis, z_axis, size);
    }
    inline void DrawAxis(const Vector3& origin, const Quat& rotation, const float size = 1.0f) {
        DrawAxis(origin, rotation, Vector3Constants::Right, Vector3Constants::Up, Vector3Constants::Forward, size);
    }
    inline void DrawAxis(const Vector3& origin, const float size = 1.0f) {
        DrawAxis(origin, Vector3Constants::Right, Vector3Constants::Up, Vector3Constants::Forward, size);
    }
    void DrawQuad(const Vector3& origin, const Quat& rotation, const Vector2& size, const Color& color);
    inline void DrawQuad(const Vector3& origin, const Vector2& size, const Color& color) {
        DrawQuad(origin, QuatConstants::Identity, size, color);
    }
    void DrawCube(const Vector3& origin, const Quat& rotation, const Vector3& mins, const Vector3& maxs, const Color& color);
    inline void DrawCube(const Vector3& origin, const Vector3& mins, const Vector3& maxs, const Color& color) {
        DrawCube(origin, QuatConstants::Identity, mins, maxs, color);
    }
    void DrawCross(const Vector3& origin, const Quat& rotation, float size, const Color& color);
    inline void DrawCross(const Vector3& origin, const float size, const Color& color) {
        DrawCross(origin, QuatConstants::Identity, size, color);
    }
    void DrawArrow(const Vector3& start, const Vector3& end, float head_size, const Color& start_color, const Color& end_color);
    inline void DrawArrow(const Vector3& start, const Vector3& end, const float head_size, const Color& color) {
        DrawArrow(start, end, head_size, color, color);
    }
}

#endif
