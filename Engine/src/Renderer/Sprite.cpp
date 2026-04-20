#include "fow/Renderer/Sprite.hpp"

#include "fow/Renderer.hpp"

namespace fow {
    void Sprite::setup_sprite() {
        if (m_pMesh == nullptr) {
            const auto result = Mesh::CreateQuad(m_pMaterial);
            if (!result.has_value()) {
                Debug::LogError("Failed to create quad mesh for sprite");
                return;
            }
            m_pMesh = result.value();
        }
    }

    Sprite::Sprite(const MaterialPtr& material, const BillboardMode mode): m_pMaterial(material), m_eBillboardMode(mode) {
        setup_sprite();
    }
    Sprite::Sprite(MaterialPtr&& material, const BillboardMode mode) noexcept: m_pMaterial(std::move(material)), m_eBillboardMode(mode) {
        setup_sprite();
    }
    Sprite::Sprite(const Sprite& sprite) : m_pMaterial(sprite.m_pMaterial), m_pMesh(sprite.m_pMesh), m_eBillboardMode(sprite.m_eBillboardMode) {
        setup_sprite();
    }
    Sprite::Sprite(Sprite&& sprite) noexcept : m_pMaterial(std::move(sprite.m_pMaterial)), m_pMesh(std::move(sprite.m_pMesh)), m_eBillboardMode(sprite.m_eBillboardMode) {
        setup_sprite();
    }

    void Sprite::set_material(const MaterialPtr& material) {
        m_pMaterial = material;
        if (m_pMesh != nullptr) {
            m_pMesh->set_material(m_pMaterial);
        }
    }
    void Sprite::draw(const Transform& transform) const {
        if (m_pMesh != nullptr) {
            Debug::AssertWarn(m_pMesh->material()->set_parameter_optional("BillboardMode", static_cast<GLuint>(m_eBillboardMode)));
            m_pMesh->draw(transform);
        }
    }

    void Sprite::draw_instances(const Vector<Transform>& transforms) const {
        if (m_pMesh != nullptr) {
            Debug::AssertWarn(m_pMesh->material()->set_parameter_optional("BillboardMode", static_cast<GLuint>(m_eBillboardMode)));
            m_pMesh->draw_instances(transforms);
        }
    }

    Result<SpritePtr> Sprite::LoadAsset(const Path& path, AssetLoaderFlags::Type flags) {
        const auto xml = Assets::LoadAsXml(path, flags);
        if (!xml.has_value()) {
            return Failure(xml.error());
        }

        const auto root = xml->child("Sprite");
        if (!root) {
            return Failure(std::format("Failed to load Sprite \"{}\": Expected root node \"Sprite\"", path));
        }
        const auto mat_node = root.child("Material");
        if (!mat_node) {
            return Failure(std::format("Expected child node \"Material\" in root node \"Sprite\""));
        }

        MaterialPtr material;
        if (const auto src_attrib = mat_node.attribute("src"); src_attrib) {
            auto mat_result = Assets::Load<Material>(src_attrib.value(), flags);
            if (!mat_result.has_value()) {
                return Failure(std::format("Failed to load Sprite \"{}\": {}", path, mat_result.error().message));
            }
            material = std::move(mat_result.value().ptr());
        } else {
            auto mat_result = Material::ParseXml(std::format("{}:internal", path), mat_node, flags);
            if (!mat_result.has_value()) {
                return Failure(std::format("Failed to load Sprite \"{}\": {}", path, mat_result.error().message));
            }
            material = std::move(mat_result.value());
        }

        auto billboard_mode = BillboardMode::None;
        if (const auto billboard_node = root.child("Billboard"); billboard_node) {
            const auto billboard_str = String(billboard_node.child_value());
            if (billboard_str.equals_any({ "yaligned", "y_aligned", "cylindrical" }, StringCompareType::CaseInsensitive)) {
                billboard_mode = BillboardMode::BillboardCylindrical;
            } else if (billboard_str.equals_any({ "spherical" }, StringCompareType::CaseInsensitive)) {
                billboard_mode = BillboardMode::BillboardSpherical;
            }
        }

        return Success<SpritePtr>(std::make_shared<Sprite>(std::move(material), billboard_mode));
    }

    Sprite2D::Sprite2D(const MaterialPtr& material): m_pMaterial(material) {
        setup_sprite();
    }
    Sprite2D::Sprite2D(MaterialPtr&& material): m_pMaterial(std::move(material)) {
        setup_sprite();
    }

    Sprite2D::Sprite2D(const Texture2DPtr& texture) : Sprite2D(Material::New("Generic2D", { { "MainTexture", texture } }).value_or(CreateRef<Material>())) { }

    Sprite2D::Sprite2D(const Sprite2D& sprite) : m_pMaterial(sprite.m_pMaterial), m_pMesh(sprite.m_pMesh) {
        setup_sprite();
    }
    Sprite2D::Sprite2D(Sprite2D&& sprite) noexcept : m_pMaterial(std::move(sprite.m_pMaterial)), m_pMesh(std::move(sprite.m_pMesh)) {
        setup_sprite();
    }

    void Sprite2D::set_material(const MaterialPtr& material) {
        m_pMaterial = material;
        setup_sprite();
    }

    void Sprite2D::draw_2d(const Rectangle& rect) const {
        m_pMesh->draw_2d(rect, m_pMaterial);
    }

    Result<Sprite2DPtr> Sprite2D::LoadAsset(const Path& path, AssetLoaderFlags::Type flags) {
        const auto xml = Assets::LoadAsXml(path, flags);
        if (!xml.has_value()) {
            return Failure(xml.error());
        }

        const auto root = xml->child("Sprite");
        if (!root) {
            return Failure(std::format("Failed to load Sprite \"{}\": Expected root node \"Sprite\"", path));
        }
        const auto mat_node = root.child("Material");
        if (!mat_node) {
            return Failure(std::format("Expected child node \"Material\" in root node \"Sprite\""));
        }

        MaterialPtr material;
        if (const auto src_attrib = mat_node.attribute("src"); src_attrib) {
            auto mat_result = Assets::Load<Material>(src_attrib.value(), flags);
            if (!mat_result.has_value()) {
                return Failure(std::format("Failed to load Sprite \"{}\": {}", path, mat_result.error().message));
            }
            material = std::move(mat_result.value().ptr());
        } else {
            auto mat_result = Material::ParseXml(std::format("{}:internal", path), mat_node, flags);
            if (!mat_result.has_value()) {
                return Failure(std::format("Failed to load Sprite \"{}\": {}", path, mat_result.error().message));
            }
            material = std::move(mat_result.value());
        }

        if (root.child("Billboard")) {
            Debug::LogWarning(std::format("Asset \"{}\" is loaded as Sprite2D, parameter \"Billboard\" will be ignored!", path));
        }

        return Success<Sprite2DPtr>(std::make_shared<Sprite2D>(std::move(material)));
    }

    void Sprite2D::setup_sprite() {
        if (m_pMesh == nullptr) {
            const auto result = Mesh::CreateQuad2D(m_pMaterial);
            if (!result.has_value()) {
                Debug::LogError("Failed to create quad mesh for sprite");
                return;
            }
            m_pMesh = result.value();
        } else {
            m_pMesh->set_material(m_pMaterial);
        }
    }

    QuadSprite2D::QuadSprite2D() : Sprite2D(Material::New("Rectangle2D", { { "MainTexture", Texture2D::DefaultWhite() } }).value_or(CreateRef<Material>())) { }

    QuadSprite2D::QuadSprite2D(const Color& color, float radius) : QuadSprite2D() {
        m_Color = color;
        m_fRadius = radius;
    }
    QuadSprite2D::QuadSprite2D(const Color& color, const Color& border_color, const float border_thickness, const float radius) : QuadSprite2D() {
        m_Color = color;
        m_fRadius = radius;
        m_BorderColor = border_color;
        m_fBorderThickness = border_thickness;
    }

    QuadSprite2D::QuadSprite2D(const Texture2DPtr& background, const Color& color, const Color& border_color, const float border_thickness, const float radius) : QuadSprite2D() {
        m_Color = color;
        m_fRadius = radius;
        m_BorderColor = border_color;
        m_fBorderThickness = border_thickness;
        m_pBackgroundTexture = background;
    }

    void QuadSprite2D::set_color(const Color& color) {
        m_Color = color;
    }
    void QuadSprite2D::set_border(const Color& color, const float thickness) {
        m_BorderColor = color;
        m_fBorderThickness = thickness;
    }
    void QuadSprite2D::set_radius(const float radius) {
        m_fRadius = radius;
    }

    void QuadSprite2D::set_background_texture(const Texture2DPtr& texture) {
        m_pBackgroundTexture = texture;
        if (m_pBackgroundTexture == nullptr) {
            m_pMaterial->set_parameter("MainTexture", Texture2D::DefaultWhite());
        }
    }

    void QuadSprite2D::draw_2d(const Rectangle& rect) const {
        m_pMaterial->set_parameter("ColorTint", m_Color);
        m_pMaterial->set_parameter("BorderColor", m_BorderColor);
        m_pMaterial->set_parameter("BorderThickness", m_fBorderThickness);
        m_pMaterial->set_parameter("Radius", m_fRadius);
        if (m_pBackgroundTexture != nullptr) {
            m_pMaterial->set_parameter("MainTexture", m_pBackgroundTexture);
        }
        Sprite2D::draw_2d(rect);
    }

    Result<QuadSprite2DPtr> QuadSprite2D::FromXml(const pugi::xml_document& doc) {
        const auto root = doc.child("QuadSprite2D");
        if (!root) {
            return Failure("Failed to load QuadSprite2D: Expected root node \"QuadSprite2D\"");
        }

        return FromXml(root);
    }
    Result<QuadSprite2DPtr> QuadSprite2D::FromXml(const pugi::xml_node& node) {
        Color color = ColorConstants::White, border_color = ColorConstants::Black;
        Texture2DPtr texture = nullptr;
        float border_thickness = 0.0f, radius = 0.0f;

        if (const auto cn = node.child("Background"); cn) {
            if (const auto attrib = cn.attribute("color"); attrib) {
                if (auto result = StringToColor(attrib.value()); !result.has_value()) {
                    Debug::LogError(std::format("Failed to parse color value \"{}\" by attribute \"color\" in node \"Background\": {}, using default value!", attrib.value(), result.error().message));
                } else {
                    color = result.value();
                }
            }
            if (const auto attrib = cn.attribute("texture"); attrib) {
                auto texture_result = Assets::Load<Texture2D>(attrib.value());
                if (!texture_result.has_value()) {
                    return Failure(std::format("Failed to load QuadSprite2D texture: {}", texture_result.error().message));
                }
                texture = texture_result.value().ptr();
            }
        }
        if (const auto cn = node.child("Border"); cn) {
            if (const auto attrib = cn.attribute("color"); attrib) {
                if (auto result = StringToColor(attrib.value()); !result.has_value()) {
                    Debug::LogError(std::format("Failed to parse color value \"{}\" used by attribute \"color\" in node \"Border\": {}, using default value!", attrib.value(), result.error().message));
                } else {
                    border_color = result.value();
                }
            }
            if (const auto attrib = cn.attribute("thickness"); attrib) {
                if (auto result = StringToFloat<float>(attrib.value()); !result.has_value()) {
                    Debug::LogError(std::format("Failed to parse float value \"{}\" used by attribute \"thickness\" in node \"Border\": {}, using default value!", attrib.value(), result.error().message));
                } else {
                    border_thickness = result.value();
                }
            }
        }
        if (const auto cn = node.child("Radius"); cn) {
            if (auto result = StringToFloat<float>(cn.child_value()); !result.has_value()) {
                Debug::LogError(std::format("Failed to parse float value \"{}\" used by node \"Radius\": {}, using default value!", cn.child_value(), result.error().message));
            } else {
                radius = result.value();
            }
        }

        return Success<QuadSprite2DPtr>(CreateRef<QuadSprite2D>(texture, color, border_color, border_thickness, radius));
    }
    Result<QuadSprite2DPtr> QuadSprite2D::LoadAsset(const Path& path, const AssetLoaderFlags::Type flags) {
        const auto xml = Assets::LoadAsXml(path, flags);
        if (!xml.has_value()) {
            return Failure(xml.error());
        }
        return FromXml(xml.value());
    }

    Font::Font(const Path& path, const uint32_t size) {
        const auto full_path = path.is_absolute() ? path : path.as_absolute(Renderer::GetBasePath() / "res");
        FT_New_Face(Renderer::FontLibrary(), full_path.as_cstr(), 0, &m_pFace);
        FT_Set_Pixel_Sizes(m_pFace, 0, size);
    }
    Font::~Font() {
        if (m_pFace != nullptr) {
            FT_Done_Face(m_pFace);
        }
    }

    Result<> Font::change_font(const Path& path, const uint32_t size) {
        return Success();
    }

    const TextAlignment TextAlignment::Default = { HorizontalAlignment::Left, VerticalAlignment::Top };
    const TextAlignment TextAlignment::Center  = { HorizontalAlignment::Center, VerticalAlignment::Centre };
    const TextAlignment TextAlignment::Centre  = { HorizontalAlignment::Center, VerticalAlignment::Centre };

    TextRenderer::TextRenderer(const String& text, const Path& font_path, float size) : TextRenderer(text, CreateRef<Font>(font_path, size)) { }

    TextRenderer::TextRenderer(const String& text, const FontPtr& font) : m_pFont(font), m_sText(text) {

    }

    void TextRenderer::set_text(const String& text) {
        m_sText = text;
    }

    void TextRenderer::set_font(const FontPtr& font) {
        m_pFont = font;
    }

    void TextRenderer::set_color(const Color& color) {
        m_Color = color;
    }

    void TextRenderer::set_alignment(const TextAlignment& alignment) {
        m_eTextAlignment = alignment;
    }

    void TextRenderer::set_alignment(const HorizontalAlignment& horizontal_alignment, const VerticalAlignment& vertical_alignment) {
        m_eTextAlignment = TextAlignment(horizontal_alignment, vertical_alignment);
    }

    void TextRenderer::set_alignment(const HorizontalAlignment& alignment) {
        m_eTextAlignment.horizontal = alignment;
    }
    void TextRenderer::set_alignment(const VerticalAlignment& vertical_alignment) {
        m_eTextAlignment.vertical = vertical_alignment;
    }

    Result<Texture2DPtr> TextRenderer::create_texture(const Vector2i& texture_size) const {
        return create_texture(texture_size, TextureInfo());
    }
    Result<Texture2DPtr> TextRenderer::create_texture(const Vector2i& texture_size, const TextureInfo& info) const {
        return create_texture(texture_size, info, nullptr);
    }
    Result<Texture2DPtr> TextRenderer::create_texture(const Vector2i& texture_size, const TexturePtr& reuse) const {
        return create_texture(texture_size, TextureInfo(), reuse);
    }
    Result<Texture2DPtr> TextRenderer::create_texture(const Vector2i& texture_size, const TextureInfo& info, const TexturePtr& reuse) const {
        if (m_pFont == nullptr) {
            return Failure(std::format("Failed to render text \"{}\": Font is not set", m_sText));
        }

        const auto face = m_pFont->m_pFace;
        const auto target_width = texture_size.x;
        const auto target_height = texture_size.y;

        // 1. Calculate the bounding box of the actual text string
        int text_width = 0;
        int max_ascent = 0;
        int max_descent = 0;

        for (const char c : m_sText) {
            if (FT_Load_Char(face, c, FT_LOAD_DEFAULT)) continue;
            text_width += face->glyph->advance.x >> 6;
            max_ascent = std::max(max_ascent, face->glyph->bitmap_top);
            max_descent = std::max(max_descent, static_cast<int>(face->glyph->bitmap.rows - face->glyph->bitmap_top));
        }
        const int text_height = max_ascent + max_descent;

        // 2. Determine starting X based on Horizontal Alignment
        int start_x = 0;
        switch (m_eTextAlignment.horizontal) {
            case HorizontalAlignment::Left:   start_x = 0; break;
            case HorizontalAlignment::Center: start_x = (target_width - text_width) / 2; break;
            case HorizontalAlignment::Right:  start_x = (target_width - text_width); break;
        }

        // 3. Determine starting Y based on Vertical Alignment
        // We align the "block", then adjust for the font baseline
        int start_y_offset = 0;
        switch (m_eTextAlignment.vertical) {
            case VerticalAlignment::Top:    start_y_offset = 0; break;
            case VerticalAlignment::Center: start_y_offset = (target_height - text_height) / 2; break;
            case VerticalAlignment::Bottom: start_y_offset = (target_height - text_height); break;
        }

        // The baseline Y position
        const int baseline_y = start_y_offset + max_ascent;

        // 4. Initialize RGBA buffer
        Vector<uint8_t> buffer(target_width * target_height * 4, 0);

        int current_x = start_x;
        for (const char c : m_sText) {
            if (FT_Load_Char(face, c, FT_LOAD_RENDER)) continue;

            const FT_GlyphSlot slot = face->glyph;

            // Calculate pixel position: baseline minus the glyph's top offset
            int glyph_y_top = baseline_y - slot->bitmap_top;

            for (uint32_t row = 0; row < slot->bitmap.rows; ++row) {
                for (uint32_t col = 0; col < slot->bitmap.width; ++col) {
                    const int pixel_x = current_x + slot->bitmap_left + col;
                    const int pixel_y = glyph_y_top + row;

                    // Bounds check to prevent buffer overflow if text exceeds target_width/height
                    if (pixel_x < 0 || pixel_x >= static_cast<int>(target_width) || pixel_y < 0 || pixel_y >= static_cast<int>(target_height))
                        continue;

                    const uint8_t alpha = slot->bitmap.buffer[row * slot->bitmap.width + col];
                    const size_t idx = (pixel_y * target_width + pixel_x) * 4;

                    buffer[idx + 0] = static_cast<uint8_t>(m_Color.r * 255);
                    buffer[idx + 1] = static_cast<uint8_t>(m_Color.g * 255);
                    buffer[idx + 2] = static_cast<uint8_t>(m_Color.b * 255);
                    buffer[idx + 3] = static_cast<uint8_t>(alpha * m_Color.a);
                }
            }
            current_x += slot->advance.x >> 6;
        }

        return Texture2D::CreateFromRawData(buffer, reuse != nullptr ? reuse->id() : 0, texture_size, info, TexturePixelFormat::RGBA, TextureInternalPixelFormat::RGBA);
    }

    BaseTextSprite::BaseTextSprite(const String& text, const FontPtr& font, const MaterialPtr& material, const IntRectangle& text_area) :
        m_pMaterial(material->make_unique_ptr()), m_textRenderer(text, font), m_textArea(text_area) { }

    TextSprite::TextSprite(const String& text, const FontPtr& font, const MaterialPtr& material, const IntRectangle& text_area, const BillboardMode& mode) : BaseTextSprite(text, font, material, text_area), m_eBillboardMode(mode) {
        TextSprite::setup_sprite();
    }

    void TextSprite::draw(const Transform& transform) const {
        if (m_pMesh == nullptr) return;
        Debug::Assert(m_pMaterial->set_parameter("MainTexture", m_pTexture));
        Debug::Assert(m_pMaterial->set_parameter("BillboardMode", static_cast<GLuint>(m_eBillboardMode)));
        m_pMesh->draw(transform);
    }
    void TextSprite::draw_instances(const Vector<Transform>& transforms) const {
        if (m_pMesh == nullptr) return;
        Debug::Assert(m_pMaterial->set_parameter("MainTexture", m_pTexture));
        Debug::Assert(m_pMaterial->set_parameter("BillboardMode", static_cast<GLuint>(m_eBillboardMode)));
        m_pMesh->draw_instances(transforms);
    }

    void TextSprite::setup_sprite() {
        if (m_pMesh == nullptr) {
            const auto result = Mesh::CreateQuad(m_pMaterial);
            if (!result.has_value()) {
                Debug::LogError("Failed to create quad mesh for sprite");
                return;
            }
            m_pMesh = result.value();
        }

        BaseTextSprite::setup_sprite();
    }

    void BaseTextSprite::set_material(const MaterialPtr& material) {
        m_pMaterial = material;
        if (m_pMesh != nullptr) {
            m_pMesh->set_material(m_pMaterial);
        }
    }

    String BaseTextSprite::text() const {
        return m_textRenderer.text();
    }

    void BaseTextSprite::set_text(const String& text) {
        m_textRenderer.set_text(text);
        setup_sprite();
    }

    void BaseTextSprite::set_font(const FontPtr& font) {
        m_textRenderer.set_font(font);
        setup_sprite();
    }

    void BaseTextSprite::set_color(const Color& color) {
        m_textRenderer.set_color(color);
        setup_sprite();
    }

    void BaseTextSprite::set_alignment(const TextAlignment& alignment) {
        m_textRenderer.set_alignment(alignment);
        setup_sprite();
    }

    void BaseTextSprite::set_text_area(const IntRectangle& rect) {
        m_textArea = rect;
        setup_sprite();
    }

    void BaseTextSprite::setup_sprite() {
        auto result = m_textRenderer.create_texture(m_textArea.size());
        if (!result.has_value()) {
            Debug::LogError(result.error().message);
            return;
        }
        m_pTexture = result.value();
    }

    TextSprite2D::TextSprite2D(const String& text, const FontPtr& font, const MaterialPtr& material, const IntRectangle& text_area): BaseTextSprite(text, font, material, text_area) {
        TextSprite2D::setup_sprite();
    }

    void TextSprite2D::draw_2d(const Rectangle& rect) const {
        if (m_pMesh == nullptr) return;
        Debug::Assert(m_pMaterial->set_parameter("MainTexture", m_pTexture));
        m_pMesh->draw_2d(rect);
    }

    void TextSprite2D::setup_sprite() {
        if (m_pMesh == nullptr) {
            const auto result = Mesh::Create2D(
                m_pMaterial,
                {
                    Vertex2D { Vector2 { 0.0f, 0.0f }, Vector2 { 0.0f, 0.0f } },
                    Vertex2D { Vector2 { 0.0f, 1.0f }, Vector2 { 0.0f, 1.0f } },
                    Vertex2D { Vector2 { 1.0f, 1.0f }, Vector2 { 1.0f, 1.0f } },
                    Vertex2D { Vector2 { 1.0f, 0.0f }, Vector2 { 1.0f, 0.0f } },
                },
                {
                    0u, 1u, 2u,
                    2u, 3u, 0u
                }
            );
            if (!result.has_value()) {
                Debug::LogError("Failed to create quad mesh for sprite");
                return;
            }
            m_pMesh = result.value();
        } else {
            m_pMesh->set_material(m_pMaterial);
        }

        BaseTextSprite::setup_sprite();
    }
}
