#ifndef FOW_EDITOR_MATERIALPREVIEW_HPP
#define FOW_EDITOR_MATERIALPREVIEW_HPP

#include "GLView.hpp"

namespace fow {
    class MaterialPreview : public GLView {
        MeshPtr m_pSphere;
    public:
        explicit MaterialPreview(const MaterialPtr& material, QWidget* parent = nullptr);

        void setMaterial(const MaterialPtr& material) const;
        [[nodiscard]] FOW_CONSTEXPR const MaterialPtr& getMaterial() const { return m_pSphere->material(); }
    };
}

#endif