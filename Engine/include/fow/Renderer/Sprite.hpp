#ifndef FOW_RENDERER_SPRITE_HPP
#define FOW_RENDERER_SPRITE_HPP

#include <SDL3_ttf/SDL_ttf.h>

#include "fow/Shared.hpp"
#include "fow/Renderer/Texture.hpp"
#include "fow/Renderer/Material.hpp"
#include "fow/Renderer/Mesh.hpp"

#include <ft2build.h>

#include "Sprite.hpp"

#include FT_FREETYPE_H

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
    class ArraySprite2D;
    using ArraySprite2DPtr = Ref<ArraySprite2D>;
    class AnimatedSprite2D;
    using AnimatedSprite2DPtr = Ref<AnimatedSprite2D>;
    class QuadSprite2D;
    using QuadSprite2DPtr = Ref<QuadSprite2D>;
    class LineSprite2D;
    using LineSprite2DPtr = Ref<LineSprite2D>;
    class NineSliceSprite2D;
    using NineSliceSprite2DPtr = Ref<NineSliceSprite2D>;

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
    protected:
        MaterialPtr m_pMaterial;
        MeshPtr m_pMesh;
    public:
        explicit Sprite2D(const MaterialPtr& material);
        explicit Sprite2D(MaterialPtr&& material);
        explicit Sprite2D(const Texture2DPtr& texture);

        Sprite2D(const Sprite2D& sprite);
        Sprite2D(Sprite2D&& sprite) noexcept;

        void set_material(const MaterialPtr& material);
        [[nodiscard]] FOW_CONSTEXPR const MaterialPtr& material() const { return m_pMaterial; }
        [[nodiscard]] FOW_CONSTEXPR bool is_valid() const { return m_pMesh != nullptr; }

        void draw_2d(const Rectangle& rect) const override;

        static Result<Sprite2DPtr> LoadAsset(const Path& path, AssetLoaderFlags::Type flags);
    private:
        void setup_sprite();
    };

    class FOW_RENDER_API ArraySprite2D : public IDrawable2D {
        MaterialPtr m_pMaterial;
        MeshPtr m_pMesh;
        std::variant<Texture2DPtr, Texture2DArrayPtr> m_pTexture;
        int m_iColumns, m_iRows;
        size_t m_uIndex = 0;
    public:
        ArraySprite2D(const Texture2DPtr& texture, int columns, int rows, size_t index = 0);
        explicit ArraySprite2D(const Texture2DArrayPtr& texture, size_t index = 0);

        void set_index(size_t index);
        [[nodiscard]] FOW_CONSTEXPR size_t index() const { return m_uIndex; }
        [[nodiscard]] size_t max_index() const;

        void set_texture(const Texture2DPtr& texture, int columns, int rows, size_t index = 0);
        void set_texture(const Texture2DArrayPtr& texture, size_t index = 0);

        void draw_2d(const Rectangle& rect) const override;

        static Result<ArraySprite2DPtr> LoadAsset(const Path& path, AssetLoaderFlags::Type flags);
    private:
        void setup_sprite();
    };

    enum class SpriteAnimationLoopingType {
        PlayOnce,
        Loop,
        PingPong
    };

    class FOW_RENDER_API AnimatedSprite2D : public ArraySprite2D, public IDrawable2DAnimated {
        float m_fIndexProgress = 0.0f;
        float m_fPlaybackSpeed = 1.0f;
        bool m_bBackwards = false;
        SpriteAnimationLoopingType m_eLoopingType = SpriteAnimationLoopingType::PlayOnce;
    public:
        AnimatedSprite2D(const Texture2DPtr& texture, const int columns, const int rows) : ArraySprite2D(texture, columns, rows) { }
        explicit AnimatedSprite2D(const Texture2DArrayPtr& texture) : ArraySprite2D(texture) { }

        void set_playback_speed(const float speed) { m_fPlaybackSpeed = speed; }
        void set_looping_type(const SpriteAnimationLoopingType looping_type) { m_eLoopingType = looping_type; }

        void draw_2d(const Rectangle& rect) const override;
        void draw_2d_and_progress_frame(const Rectangle& rect) override;
    };

    class FOW_RENDER_API QuadSprite2D : public Sprite2D {
        Color m_Color = ColorConstants::White;
        Color m_BorderColor;
        float m_fBorderThickness = 0.0f;
        float m_fRadius = 0.0f;
        Texture2DPtr m_pBackgroundTexture;
    public:
        QuadSprite2D();
        explicit QuadSprite2D(const Color& color, float radius = 0.0f);
        QuadSprite2D(const Color& color, const Color& border_color, float border_thickness, float radius = 0.0f);
        QuadSprite2D(const Texture2DPtr& background, const Color& color, const Color& border_color, float border_thickness, float radius = 0.0f);
        explicit QuadSprite2D(const MaterialPtr& material) : Sprite2D(material) { }
        explicit QuadSprite2D(MaterialPtr&& material) : Sprite2D(std::move(material)) { }

        [[nodiscard]] FOW_CONSTEXPR Color color() const { return m_Color; }
        [[nodiscard]] FOW_CONSTEXPR Color border_color() const { return m_BorderColor; }
        [[nodiscard]] FOW_CONSTEXPR float border_thickness() const { return m_fBorderThickness; }
        [[nodiscard]] FOW_CONSTEXPR float radius() const { return m_fRadius; }

        void set_color(const Color& color);
        void set_border(const Color& color, float thickness);
        void set_radius(float radius);
        void set_background_texture(const Texture2DPtr& texture);

        void draw_2d(const Rectangle& rect) const override;

        static Result<QuadSprite2DPtr> FromXml(const pugi::xml_document& doc);
        static Result<QuadSprite2DPtr> FromXml(const pugi::xml_node& node);
        static Result<QuadSprite2DPtr> FromXmlWithConstants(const pugi::xml_node& node, const HashMap<String, String>& constants);
        static Result<QuadSprite2DPtr> LoadAsset(const Path& path, AssetLoaderFlags::Type flags);
    };

    enum class Orientation {
        Horizontal,
        Vertical
    };

    class FOW_RENDER_API LineSprite2D : public IDrawable2D {
        MeshPtr m_pMeshStart;
        MeshPtr m_pMeshMiddle;
        MeshPtr m_pMeshEnd;
        Texture2DPtr m_pTexture;
        Orientation m_eOrientation;
    public:
        explicit LineSprite2D(const Texture2DPtr& texture, Orientation orientation = Orientation::Horizontal);
        explicit LineSprite2D(Texture2DPtr&& texture, Orientation orientation = Orientation::Horizontal) noexcept;

        void set_texture(const Texture2DPtr& texture);
        void set_texture(Texture2DPtr&& texture) noexcept;
        void set_orientation(Orientation orientation);

        [[nodiscard]] FOW_CONSTEXPR const Texture2DPtr& texture() const { return m_pTexture; }
        [[nodiscard]] FOW_CONSTEXPR Orientation orientation() const { return m_eOrientation; }
        [[nodiscard]] FOW_CONSTEXPR bool is_valid() const {
            return m_pMeshStart != nullptr && m_pMeshMiddle != nullptr && m_pMeshEnd != nullptr && m_pTexture != nullptr;
        }

        void draw_2d(const Rectangle& rect) const override;
    private:
        void setup_sprite();
    };

    class FOW_RENDER_API NineSliceSprite2D : public IDrawable2D {
        MeshPtr m_pMeshes[9];
        Texture2DPtr m_pTexture;
    public:
        explicit NineSliceSprite2D(const Texture2DPtr& texture);
        explicit NineSliceSprite2D(Texture2DPtr&& texture) noexcept;

        void set_texture(const Texture2DPtr& texture);
        void set_texture(Texture2DPtr&& texture) noexcept;

        [[nodiscard]] FOW_CONSTEXPR const Texture2DPtr& texture() const { return m_pTexture; }
        [[nodiscard]] FOW_CONSTEXPR bool is_valid() const {
            return m_pMeshes[0] != nullptr && m_pMeshes[1] != nullptr && m_pMeshes[2] != nullptr &&
                   m_pMeshes[3] != nullptr && m_pMeshes[4] != nullptr && m_pMeshes[5] != nullptr &&
                   m_pMeshes[6] != nullptr && m_pMeshes[7] != nullptr && m_pMeshes[8] != nullptr &&
                   m_pTexture != nullptr;
        }

        void draw_2d(const Rectangle& rect) const override;
    private:
        void setup_sprite();
    };

    class FOW_RENDER_API Font final {
        FT_Face m_pFace;
    public:
        Font(const Path& path, uint32_t size);
        ~Font();

        [[nodiscard]] FOW_RENDER_API constexpr bool is_valid() const { return m_pFace != nullptr; }

        Result<> change_font(const Path& path, uint32_t size);

        friend class TextRenderer;
        friend class BaseTextSprite;
        friend class TextSprite;
        friend class TextSprite2D;
    };

    enum class HorizontalAlignment {
        Left, Center, Right,
        Centre = Center
    };
    enum class VerticalAlignment {
        Top, Center, Bottom,
        Centre = Center
    };
    struct FOW_RENDER_API TextAlignment {
        HorizontalAlignment horizontal;
        VerticalAlignment vertical;

        FOW_CONSTEXPR TextAlignment(const HorizontalAlignment horizontal, const VerticalAlignment vertical) : horizontal(horizontal), vertical(vertical) { }
        FOW_CONSTEXPR TextAlignment() : horizontal(HorizontalAlignment::Left), vertical(VerticalAlignment::Top) { }

        static const TextAlignment Default;
        static const TextAlignment Center;
        static const TextAlignment Centre;
    };

    class FOW_RENDER_API TextRenderer {
        FontPtr m_pFont;
        String m_sText;
        Color m_Color = ColorConstants::White;
        TextAlignment m_eTextAlignment = { HorizontalAlignment::Left, VerticalAlignment::Top };
    public:
        TextRenderer(const String& text, const Path& font_path, float size);
        TextRenderer(const String& text, const FontPtr& font);

        void set_text(const String& text);
        void set_font(const FontPtr& font);
        void set_color(const Color& color);
        void set_alignment(const TextAlignment& alignment);
        void set_alignment(const HorizontalAlignment& horizontal_alignment, const VerticalAlignment& vertical_alignment);
        void set_alignment(const HorizontalAlignment& alignment);
        void set_alignment(const VerticalAlignment& vertical_alignment);

        [[nodiscard]] FOW_CONSTEXPR Color color() const { return m_Color; }
        [[nodiscard]] FOW_CONSTEXPR const String& text() const { return m_sText; }
        [[nodiscard]] FOW_CONSTEXPR const FontPtr& font() const { return m_pFont; }
        [[nodiscard]] FOW_CONSTEXPR const TextAlignment& alignment() const { return m_eTextAlignment; }

        [[nodiscard]] FOW_CONSTEXPR bool is_valid() const { return m_pFont != nullptr; }

        [[nodiscard]] Result<Texture2DPtr> create_texture(const Vector2i& texture_size) const;
        [[nodiscard]] Result<Texture2DPtr> create_texture(const Vector2i& texture_size, const TextureInfo& info) const;
        [[nodiscard]] Result<Texture2DPtr> create_texture(const Vector2i& texture_size, const TexturePtr& reuse) const;
        [[nodiscard]] Result<Texture2DPtr> create_texture(const Vector2i& texture_size, const TextureInfo& info, const TexturePtr& reuse) const;
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
        [[nodiscard]] FOW_CONSTEXPR const FontPtr& font() const { return m_textRenderer.font(); }

        void set_color(const Color& color);
        [[nodiscard]] FOW_CONSTEXPR Color color() const { return m_textRenderer.color(); }

        void set_alignment(const TextAlignment& alignment);
        [[nodiscard]] FOW_CONSTEXPR const TextAlignment& alignment() const { return m_textRenderer.alignment(); }

        [[nodiscard]] FOW_CONSTEXPR const IntRectangle& text_area() const { return m_textArea; }
        void set_text_area(const IntRectangle& rect);

        void update_texture();

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