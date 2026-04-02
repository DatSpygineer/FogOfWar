#include "fow/Renderer/Sprite.hpp"

namespace fow {
    static void setup_sprite(MeshPtr& mesh, const MaterialPtr& material) {
        if (mesh == nullptr) {
            const auto result = Mesh::CreateQuad(material);
            if (!result.has_value()) {
                Debug::LogError("Failed to create quad mesh for sprite");
                return;
            }
            mesh = result.value();
        }
    }

    void Sprite::set_material(const MaterialPtr& material) {
        m_pMaterial = material;
        if (m_pMesh != nullptr) {
            m_pMesh->material() = material;
        }
    }
    void Sprite::draw(const Transform& transform) {
        setup_sprite(m_pMesh, m_pMaterial);

        if (m_pMesh != nullptr) {
            Debug::AssertWarn(m_pMesh->material()->set_parameter_optional("BillboardMode", static_cast<GLuint>(m_eBillboardMode)));
            m_pMesh->draw(transform);
        }
    }

    void Sprite::draw_instances(const Vector<Transform>& transforms) {
        setup_sprite(m_pMesh, m_pMaterial);

        if (m_pMesh != nullptr) {
            Debug::AssertWarn(m_pMesh->material()->set_parameter_optional("BillboardMode", static_cast<GLuint>(m_eBillboardMode)));
            m_pMesh->draw_instances(transforms);
        }
    }
}
