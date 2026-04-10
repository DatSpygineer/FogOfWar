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

    Font::Font(const Path& path, const size_t size) {
        const auto full_path = path.is_absolute() ? path : path.as_absolute(Renderer::GetBasePath() / "res");

        m_pFont = TTF_OpenFont(full_path.as_cstr(), size);
        Debug::Assert(m_pFont != nullptr, std::format("Failed to open font \"{}\": {}", full_path, SDL_GetError()));
    }
    Font::~Font() {
        if (m_pFont != nullptr) {
            TTF_CloseFont(m_pFont);
        }
    }

    TextSprite::TextSprite(const String& text, const Font& font, const MaterialPtr& material, const IntRectangle& text_area) :
        m_pMaterial(material->make_unique_ptr()), m_pText(nullptr), m_textArea(text_area) {
        m_pText = TTF_CreateText(Renderer::TextEngine(), font.m_pFont, text.as_cstr(), text.size());
        if (m_pText == nullptr) {
            Debug::LogError(std::format("Failed to create TextSprite: {}", SDL_GetError()));
        }
    }
    TextSprite::~TextSprite() {
        if (m_pText != nullptr) {
            TTF_DestroyText(m_pText);
        }
    }

    void TextSprite::set_material(const MaterialPtr& material) {
        m_pMaterial = material;
        if (m_pMesh != nullptr) {
            m_pMesh->set_material(m_pMaterial);
        }
    }

    String TextSprite::text() const {
        return m_pText != nullptr ? m_pText->text : "";
    }

    void TextSprite::set_text(const String& text) {
        if (m_pText != nullptr && !text.equals(m_pText->text)) {
            if (!TTF_SetTextString(m_pText, text.as_cstr(), text.size())) {
                Debug::LogError(std::format("Failed to set font for TTF_Text object: {}", SDL_GetError()));
            } else {
                setup_sprite(); // Update texture
            }
        }
    }

    void TextSprite::set_font(const Font& font) {
        if (m_pText != nullptr) {
            if (!TTF_SetTextFont(m_pText, font.m_pFont)) {
                Debug::LogError(std::format("Failed to set font for TTF_Text object: {}", SDL_GetError()));
            } else {
                setup_sprite(); // Update texture
            }
        }
    }

    int TextSprite::text_wrap_width() const {
        if (m_pText != nullptr) {
            int wrap_width;
            if (TTF_GetTextWrapWidth(m_pText, &wrap_width)) {
                return wrap_width;
            }
            Debug::LogError(std::format("Failed to get text wrap width: {}", SDL_GetError()));
        }
        return 0;
    }

    void TextSprite::set_text_wrap_width(const int width) {
        if (m_pText != nullptr) {
            TTF_SetTextWrapWidth(m_pText, width);
            setup_sprite();
        }
    }

    void TextSprite::draw_2d(const Rectangle& rect) const {
        if (m_pMesh == nullptr) return;
        m_pMesh->draw_2d(rect);
    }

    void TextSprite::setup_sprite() {
        if (m_pMesh == nullptr) {
            const auto result = Mesh::CreateQuad2D(m_pMaterial);
            if (!result.has_value()) {
                Debug::LogError("Failed to create quad mesh for sprite");
                return;
            }
            m_pMesh = result.value();
        }

        if (m_pText != nullptr) {
            const auto surface = SDL_CreateSurface(m_textArea.width, m_textArea.height, SDL_PIXELFORMAT_RGBA8888);
            if (surface == nullptr) {
                Debug::LogError(std::format("Failed to create text surface: {}", SDL_GetError()));
                return;
            }

            if (!TTF_DrawSurfaceText(m_pText, m_textArea.x, m_textArea.y, surface)) {
                Debug::LogError(std::format("Failed to draw text: {}", SDL_GetError()));
                SDL_DestroySurface(surface);
                return;
            }

            const auto texture_result = Texture2D::FromSDLSurface(surface, *m_pTexture, TextureInfo { });
            if (!Debug::Assert(texture_result)) {
                SDL_DestroySurface(surface);
                return;
            }

            m_pTexture = texture_result.value();
            Debug::Assert(m_pMaterial->set_parameter("MainTexture", m_pTexture));
        }
    }
}
