#ifndef FOW_RENDERER_SPRITE_HPP
#define FOW_RENDERER_SPRITE_HPP

#include <SDL3_ttf/SDL_ttf.h>

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

    class Sprite;
    using SpritePtr = Ref<Sprite>;

    class Sprite2D;
    using Sprite2DPtr = Ref<Sprite2D>;

    class TextSprite;
    using TextSpritePtr = Ref<TextSprite>;

    class FOW_RENDER_API Sprite : public IDrawable3D, IDrawable3DInstanced {
        MaterialPtr m_pMaterial;
        MeshPtr m_pMesh;
        BillboardMode m_eBillboardMode;
    public:
        explicit Sprite(const MaterialPtr& material, BillboardMode mode = BillboardMode::None);
        explicit Sprite(MaterialPtr&& material, BillboardMode mode = BillboardMode::None) noexcept;

        Sprite(const Sprite& sprite);
        Sprite(Sprite&& sprite) noexcept;

        Sprite& operator=(const Sprite& sprite) = default;
        Sprite& operator=(Sprite&& sprite) noexcept = default;

        void set_material(const MaterialPtr& material);
        [[nodiscard]] FOW_CONSTEXPR const MaterialPtr& material() const { return m_pMaterial; }
        inline void set_billboard_mode(const BillboardMode mode) { m_eBillboardMode = mode; }
        [[nodiscard]] FOW_CONSTEXPR BillboardMode billboard_mode() const { return m_eBillboardMode; }
        [[nodiscard]] FOW_CONSTEXPR bool is_valid() const { return m_pMesh != nullptr; }

        void draw(const Transform& transform) const override;
        void draw_instances(const Vector<Transform>& transforms) const override;

        static Result<SpritePtr> LoadAsset(const Path& path, AssetLoaderFlags::Type flags);
    private:
        void setup_sprite();
    };

    class FOW_RENDER_API Sprite2D : public IDrawable2D {
        MaterialPtr m_pMaterial;
        MeshPtr m_pMesh;
    public:
        explicit Sprite2D(const MaterialPtr& material);
        explicit Sprite2D(MaterialPtr&& material);

        void set_material(const MaterialPtr& material);
        [[nodiscard]] FOW_CONSTEXPR const MaterialPtr& material() const { return m_pMaterial; }
        [[nodiscard]] FOW_CONSTEXPR bool is_valid() const { return m_pMesh != nullptr; }

        void draw_2d(const Rectangle& rect) const override;

        static Result<Sprite2DPtr> LoadAsset(const Path& path, AssetLoaderFlags::Type flags);
    private:
        void setup_sprite();
    };

    class FOW_RENDER_API Font final {
        TTF_Font* m_pFont;
    public:
        Font(const Path& path, size_t size);
        ~Font();

        [[nodiscard]] FOW_RENDER_API constexpr bool is_valid() const { return m_pFont != nullptr; }

        friend class TextSprite;
    };

    class FOW_RENDER_API TextSprite : public IDrawable2D {
        Texture2DPtr m_pTexture;
        MaterialPtr m_pMaterial;
        MeshPtr m_pMesh;
        TTF_Text* m_pText;
        IntRectangle m_textArea;
    public:
        TextSprite(const String& text, const Font& font, const MaterialPtr& material, const IntRectangle& text_area);
        ~TextSprite() override;

        [[nodiscard]] FOW_CONSTEXPR bool is_valid() const { return m_pMesh != nullptr && m_pText != nullptr; }

        void set_material(const MaterialPtr& material);
        [[nodiscard]] FOW_CONSTEXPR const MaterialPtr& material() const { return m_pMaterial; }

        [[nodiscard]] String text() const;
        void set_text(const String& text);

        void set_font(const Font& font);

        [[nodiscard]] int text_wrap_width() const;
        void set_text_wrap_width(int width);

        void draw_2d(const Rectangle& rect) const override;
    private:
        void setup_sprite();
    };
}

#endif