#include "MaterialPreview.hpp"

namespace fow {
    MaterialPreview::MaterialPreview(const MaterialPtr& material, QWidget* parent) : GLView(parent) {
        m_pSphere = Mesh::CreateSphere(material, 1.0f, 16).value();
    }

    void MaterialPreview::setMaterial(const MaterialPtr& material) const {
        m_pSphere->material() = material;
    }
}
