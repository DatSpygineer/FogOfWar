#ifndef FOW_EDITOR_VIEWPORT_HPP
#define FOW_EDITOR_VIEWPORT_HPP

#include "GLView.hpp"

namespace fow {
    class EditorViewport : public GLView {
        bool m_bDrawGrid = false;
        bool m_bUpdateGrid = false;
        glm::ivec2 m_GridSize = glm::ivec2(64);
        MeshPtr m_pGridMesh;
        MaterialPtr m_pGridMaterial;
    public:
        explicit EditorViewport(QWidget *parent) : GLView(parent) { }

        void initializeGL() override;
        void paintGL() override;

        void setDrawGrid(bool drawGrid);
        void setGridSize(const glm::ivec2& size);
        void updateGridMesh();

        [[nodiscard]] FOW_CONSTEXPR bool drawGrid() const { return m_bDrawGrid; }
        [[nodiscard]] FOW_CONSTEXPR glm::ivec2 gridSize() const { return m_GridSize; }
    };
}

#endif