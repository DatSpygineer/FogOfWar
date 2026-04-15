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

    class Font;
    using FontPtr = Ref<Font>;

    class TextSprite;
    using TextSpritePtr = Ref<TextSprite>;

    class TextSprite2D;
    using TextSprite2DPtr = Ref<TextSprite2D>;

    class FOW_RENDER_API Sprite : public IDrawable3D, public IDrawable3DInstanced {
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
        Font(const Path& path, float size);
        ~Font();

        [[nodiscard]] FOW_RENDER_API constexpr bool is_valid() const { return m_pFont != nullptr; }

        Result<> change_font(const Path& path, float size);

        friend class TextRenderer;
        friend class BaseTextSprite;
        friend class TextSprite;
        friend class TextSprite2D;
    };

    class FOW_RENDER_API TextRenderer {
        FontPtr m_pFont;
        TTF_Text* m_pText;
        bool m_bWarpVisibleWhitespace;
    public:
        TextRenderer(const String& text, const Path& font_path, float size);
        TextRenderer(const String& text, const FontPtr& font);
        ~TextRenderer();

        void set_text(const String& text);
        void set_font(const FontPtr& font);
        void set_color(const Color& color);
        void set_wrap_width(int width);
        void set_warp_visible_whitespace(bool visible);

        [[nodiscard]] Color color() const;
        [[nodiscard]] int wrap_width() const;
        [[nodiscard]] bool warp_visible_whitespace() const;
        [[nodiscard]] String text() const;
        [[nodiscard]] const FontPtr& font() const;

        [[nodiscard]] FOW_CONSTEXPR bool is_valid() const { return m_pText != nullptr; }

        [[nodiscard]] Result<Texture2DPtr> create_texture(const IntRectangle& rect) const;
        [[nodiscard]] Result<Texture2DPtr> create_texture(const IntRectangle& rect, const TexturePtr& reuse) const;
    };

    class FOW_RENDER_API BaseTextSprite {
    protected:
        Texture2DPtr m_pTexture = nullptr;
        MaterialPtr m_pMaterial = nullptr;
        MeshPtr m_pMesh = nullptr;
        TextRenderer m_textRenderer;
        IntRectangle m_textArea = { 0, 0, 128, 128 };
    public:
        BaseTextSprite(const String& text, const FontPtr& font, const MaterialPtr& material, const IntRectangle& text_area);
        virtual ~BaseTextSprite() = default;

        [[nodiscard]] FOW_CONSTEXPR bool is_valid() const { return m_pMesh != nullptr && m_textRenderer.is_valid(); }

        void set_material(const MaterialPtr& material);
        [[nodiscard]] FOW_CONSTEXPR const MaterialPtr& material() const { return m_pMaterial; }

        [[nodiscard]] String text() const;
        void set_text(const String& text);

        void set_font(const FontPtr& font);

        [[nodiscard]] int text_wrap_width() const;
        void set_text_wrap_width(int width);

        [[nodiscard]] FOW_CONSTEXPR const IntRectangle& text_area() const { return m_textArea; }
        void set_text_area(const IntRectangle& rect);

    protected:
        virtual void setup_sprite();
    };

    class FOW_RENDER_API TextSprite : public BaseTextSprite, public IDrawable3D, public IDrawable3DInstanced {
        BillboardMode m_eBillboardMode;
    public:
        TextSprite(const String& text, const FontPtr& font, const MaterialPtr& material, const IntRectangle& text_area, const BillboardMode& mode = BillboardMode::None);

        inline void set_billboard_mode(const BillboardMode mode) { m_eBillboardMode = mode; }
        [[nodiscard]] FOW_CONSTEXPR BillboardMode get_billboard_mode() const { return m_eBillboardMode; }

        void draw(const Transform& transform) const override;
        void draw_instances(const Vector<Transform>& transforms) const override;
    protected:
        void setup_sprite() override;
    };

    class FOW_RENDER_API TextSprite2D : public BaseTextSprite, public IDrawable2D {
    public:
        TextSprite2D(const String& text, const FontPtr& font, const MaterialPtr& material, const IntRectangle& text_area);

        void draw_2d(const Rectangle& rect) const override;
    protected:
        void setup_sprite() override;
    };
}

#endif