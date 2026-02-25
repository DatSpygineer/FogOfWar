#include "EditorViewport.hpp"

#include <QMessageBox>

namespace fow {
    void EditorViewport::initializeGL() {
        GLView::initializeGL();

        m_pGridMaterial = std::make_shared<Material>(
            Shader::FromCache("UnlitGeneric"),
            HashMap<String, MaterialParameterValue> {
                { "MainTexture", Texture::DefaultWhite() },
                { "ColorTint", glm::vec4(1.0) }
            }
        );
    }

    void EditorViewport::paintGL() {
        GLView::paintGL();
        if (m_bDrawGrid) {
            if (m_bUpdateGrid || m_pGridMesh == nullptr) {
                auto mb = MeshBuilder { MeshPrimitive::Triangles, m_pGridMaterial };
                for (uint32_t i = 0; i < static_cast<uint32_t>((m_GridSize.x + 1) * (m_GridSize.y + 1)); i++) {
                    const auto x = i % m_GridSize.x;
                    const auto y = i / m_GridSize.x;
                    mb.append(glm::vec3 { x, 0.01, y }, glm::vec3 { 0.0, 1.0, 0.0 }, glm::vec3 { 0.0, 1.0, 0.0 }, glm::vec3 { 0.0, 1.0, 0.0 }, glm::vec2 { x % 2, y % 2 });
                }

                const auto result = mb.create_mesh(MeshDrawMode::StaticDraw);
                if (!result.has_value()) {
                    QMessageBox::critical(this, "Error!", std::format("Failed to create grid mesh: {}", result.error().message).c_str());
                } else {
                    m_pGridMesh = result.value();
                }
                m_bUpdateGrid = false;
            }

            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            m_pGridMesh->draw();
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
    }

    void EditorViewport::setDrawGrid(const bool drawGrid) {
        m_bDrawGrid = drawGrid;
        m_bUpdateGrid = true;
    }

    void EditorViewport::setGridSize(const glm::ivec2& size) {
        m_GridSize = size;
        m_bUpdateGrid = true;
    }

    void EditorViewport::updateGridMesh() {
        m_bUpdateGrid = true;
    }
}
