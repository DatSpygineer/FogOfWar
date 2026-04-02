#ifndef FOW_RENDERER_SPRITE_HPP
#define FOW_RENDERER_SPRITE_HPP

#include "fow/Shared.hpp"
#include "fow/Renderer/Texture.hpp"
#include "fow/Renderer/Material.hpp"
#include "fow/Renderer/Mesh.hpp"

namespace fow {
    enum class BillboardMode {
        None,
        BillboardCylindrical,
        BillboardSpherical
    };

    class FOW_RENDER_API Sprite {
        MaterialPtr m_pMaterial;
        MeshPtr m_pMesh;
        BillboardMode m_eBillboardMode;
    public:
        explicit Sprite(const MaterialPtr& material, const BillboardMode mode = BillboardMode::None) : m_pMaterial(material), m_eBillboardMode(mode) { }
        explicit Sprite(MaterialPtr&& material, const BillboardMode mode = BillboardMode::None) noexcept : m_pMaterial(std::move(material)), m_eBillboardMode(mode) { }
        Sprite(const Sprite& sprite) = delete;
        Sprite(Sprite&& sprite) noexcept : m_pMaterial(std::move(sprite.m_pMaterial)), m_pMesh(std::move(sprite.m_pMesh)), m_eBillboardMode(sprite.m_eBillboardMode) { }

        void set_material(const MaterialPtr& material);
        [[nodiscard]] FOW_CONSTEXPR const MaterialPtr& material() const { return m_pMaterial; }
        inline void set_billboard_mode(const BillboardMode mode) { m_eBillboardMode = mode; }
        [[nodiscard]] FOW_CONSTEXPR BillboardMode billboard_mode() const { return m_eBillboardMode; }

        void draw(const Transform& transform);
        void draw_instances(const Vector<Transform>& transforms);
    };
    using SpritePtr = SharedPtr<Sprite>;
}

#endif