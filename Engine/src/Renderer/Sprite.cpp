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
    Sprite::Sprite(Sprite&& sprite) noexcept: m_pMaterial(std::move(sprite.m_pMaterial)), m_pMesh(std::move(sprite.m_pMesh)), m_eBillboardMode(sprite.m_eBillboardMode) {
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

    void Sprite2D::set_material(const MaterialPtr& material) {
        m_pMaterial = material;
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
        }
    }

    Font::Font(const Path& path, const float size) {
        const auto full_path = path.is_absolute() ? path : path.as_absolute(Renderer::GetBasePath() / "res");

        m_pFont = TTF_OpenFont(full_path.as_cstr(), size);
        Debug::Assert(m_pFont != nullptr, std::format("Failed to open font \"{}\": {}", full_path, SDL_GetError()));
    }
    Font::~Font() {
        if (m_pFont != nullptr) {
            TTF_CloseFont(m_pFont);
        }
    }

    Result<> Font::change_font(const Path& path, const float size) {
        const auto full_path = path.is_absolute() ? path : path.as_absolute(Renderer::GetBasePath() / "res");
        const auto font = TTF_OpenFont(full_path.as_cstr(), size);
        if (font == nullptr) {
            return Failure(std::format("Failed to open font \"{}\": {}", full_path, SDL_GetError()));
        }
        TTF_CloseFont(m_pFont);
        m_pFont = font;
        return Success();
    }

    TextRenderer::TextRenderer(const String& text, const Path& font_path, float size) : TextRenderer(text, CreateRef<Font>(font_path, size)) { }

    TextRenderer::TextRenderer(const String& text, const FontPtr& font) : m_pFont(font), m_pText(nullptr), m_bWarpVisibleWhitespace(false) {
        m_pText = TTF_CreateText(Renderer::TextEngine(), font->m_pFont, text.as_cstr(), text.size());
        TTF_SetTextColor(m_pText, 0xFF, 0xFF, 0xFF, 0xFF);
    }
    TextRenderer::~TextRenderer() {
        if (m_pText != nullptr) {
            TTF_DestroyText(m_pText);
        }
    }

    void TextRenderer::set_text(const String& text) {
        if (m_pText != nullptr && !text.equals(m_pText->text)) {
            if (!TTF_SetTextString(m_pText, text.as_cstr(), text.size())) {
                Debug::LogError(std::format("Failed to set text for TTF_Text object: {}", SDL_GetError()));
            }
        }
    }

    void TextRenderer::set_font(const FontPtr& font) {
        if (m_pText != nullptr) {
            if (!TTF_SetTextFont(m_pText, font->m_pFont)) {
                Debug::LogError(std::format("Failed to set font for TTF_Text object: {}", SDL_GetError()));
            }
        }
    }

    void TextRenderer::set_color(const Color& color) {
        if (m_pText != nullptr) {
            if (!TTF_SetTextColorFloat(m_pText, color.r, color.g, color.b, color.a)) {
                Debug::LogError(std::format("Failed to set color for TTF_Text object: {}", SDL_GetError()));
            }
        }
    }

    void TextRenderer::set_wrap_width(const int width) {
        if (m_pText != nullptr) {
            if (!TTF_SetTextWrapWidth(m_pText, width)) {
                Debug::LogError(std::format("Failed to set text wrap width for TTF_Text object: {}", SDL_GetError()));
            }
        }
    }

    void TextRenderer::set_warp_visible_whitespace(const bool visible) {
        if (m_pText != nullptr) {
            if (!TTF_SetTextWrapWhitespaceVisible(m_pText, visible)) {
                Debug::LogError(std::format("Failed to set text wrap whitespace visible for TTF_Text object: {}", SDL_GetError()));
            }
            m_bWarpVisibleWhitespace = visible;
        }
    }

    Color TextRenderer::color() const {
        Color color;
        TTF_GetTextColorFloat(m_pText, &color.r, &color.g, &color.b, &color.a);
        return color;
    }

    int TextRenderer::wrap_width() const {
        int wrap_width;
        TTF_GetTextWrapWidth(m_pText, &wrap_width);
        return wrap_width;
    }

    bool TextRenderer::warp_visible_whitespace() const {
        return m_bWarpVisibleWhitespace;
    }

    String TextRenderer::text() const {
        return m_pText != nullptr ? m_pText->text : "";
    }

    const FontPtr& TextRenderer::font() const {
        return m_pFont;
    }

    Result<Texture2DPtr> TextRenderer::create_texture(const IntRectangle& rect) const {
        return create_texture(rect, nullptr);
    }

    Result<Texture2DPtr> TextRenderer::create_texture(const IntRectangle& rect, const TexturePtr& reuse) const {
        if (!is_valid()) {
            return Failure("TextRenderer is not initialized");
        }

        if (rect.width <= 0 || rect.height <= 0) {
            return Failure("Invalid text area: Size (both width and height) must be non-zero, positive!");
        }

        const auto surface = SDL_CreateSurface(rect.width, rect.height, SDL_PIXELFORMAT_RGBA8888);
        if (surface == nullptr) {
            const char* error = SDL_GetError();
            return Failure(std::format("Failed to create text surface: {}", error));
        }

        if (!TTF_DrawSurfaceText(m_pText, rect.x, rect.y, surface)) {
            const char* error = SDL_GetError();
            SDL_DestroySurface(surface);
            return Failure(std::format("Failed to draw text: {}", error));
        }

        const auto texture_result = reuse != nullptr
            ? Texture2D::FromSDLSurface(surface, *reuse, TextureInfo { })
            : Texture2D::FromSDLSurface(surface, TextureInfo { });

        return texture_result;
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

    int BaseTextSprite::text_wrap_width() const {
        return m_textRenderer.wrap_width();
    }

    void BaseTextSprite::set_text_wrap_width(const int width) {
        m_textRenderer.set_wrap_width(width);
        setup_sprite();
    }

    void BaseTextSprite::set_text_area(const IntRectangle& rect) {
        m_textArea = rect;
        setup_sprite();
    }

    void BaseTextSprite::setup_sprite() {
        auto result = m_textRenderer.create_texture(m_textArea, m_pTexture);
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
            const auto result = Mesh::CreateQuad2D(m_pMaterial);
            if (!result.has_value()) {
                Debug::LogError("Failed to create quad mesh for sprite");
                return;
            }
            m_pMesh = result.value();
        }

        BaseTextSprite::setup_sprite();
    }
}
