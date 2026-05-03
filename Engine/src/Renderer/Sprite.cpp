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
        m_pMaterial->set_parameter("UVOffset", Vector2(0.0));
        m_pMaterial->set_parameter("UVScale",  Vector2(1.0));
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

    ArraySprite2D::ArraySprite2D(const Texture2DPtr& texture, const int columns, const int rows, const size_t index)
        : m_pMaterial(ResultValueOrThrow(Material::New("Generic2D"))), m_pTexture(texture), m_iColumns(columns), m_iRows(rows) {
        set_texture(texture, m_iColumns, m_iRows, index);
    }
    ArraySprite2D::ArraySprite2D(const Texture2DArrayPtr& texture, const size_t index)
        : m_pMaterial(ResultValueOrThrow(Material::New("Array2D"))), m_pTexture(texture), m_iColumns(1), m_iRows(texture->depth()) {
        set_texture(texture, index);
    }

    void ArraySprite2D::set_index(const size_t index) {
        m_uIndex = index;
        if (m_pTexture.index() == 0) {
            const auto tex = std::get<0>(m_pTexture);
            const auto tex_width = tex->width();
            const auto tex_height = tex->height();

            if (tex_width <= 0 || tex_height <= 0) {
                Debug::LogError("Failed to set ArraySprite2D texture index: Texture width or height must be positive!");
                return;
            }

            const auto frame_width  = tex_width / m_iColumns;
            const auto frame_height = tex_height / m_iRows;
            const auto uv_offset = Vector2 {
                static_cast<float>(frame_width * (m_uIndex % m_iColumns)) / static_cast<float>(tex_width),
                static_cast<float>(frame_height * (m_uIndex / m_iColumns)) / static_cast<float>(tex_height)
            };
            const auto uv_scale = Vector2 {
                static_cast<float>(frame_width) / static_cast<float>(tex_width),
                static_cast<float>(frame_height) / static_cast<float>(tex_height)
            };

            Debug::LogInfo(std::format("Setting index to {}, offset ({}, {}), scale ({}, {})", m_uIndex, uv_offset.x, uv_offset.y, uv_scale.x, uv_scale.y));

            m_pMaterial->set_parameter("UVOffset", uv_offset);
            m_pMaterial->set_parameter("UVScale", uv_scale);
        } else {
            m_pMaterial->set_parameter("FrameIndex", static_cast<GLint>(m_uIndex));
        }
        setup_sprite();
    }

    size_t ArraySprite2D::max_index() const {
        switch (m_pTexture.index()) {
            case 0: {
                return m_iColumns * m_iRows;
            }
            case 1: {
                const auto texture = std::get<1>(m_pTexture);
                return texture != nullptr ? texture->depth() : 0;
            }
            default: return 0;
        }
    }

    void ArraySprite2D::set_texture(const Texture2DPtr& texture, const int columns, const int rows, const size_t index) {
        m_pTexture = texture;
        m_iColumns = columns;
        m_iRows = rows;
        m_pMaterial->set_parameter("MainTexture", texture);
        set_index(index);
    }
    void ArraySprite2D::set_texture(const Texture2DArrayPtr& texture, const size_t index) {
        m_pTexture = texture;
        m_iColumns = 1;
        m_iRows = texture->depth();
        m_pMaterial->set_parameter("MainTexture", texture);
        set_index(index);
    }

    void ArraySprite2D::draw_2d(const Rectangle& rect) const {
        m_pMesh->draw_2d(rect);
    }

    Result<ArraySprite2DPtr> ArraySprite2D::LoadAsset(const Path& path, const AssetLoaderFlags::Type flags) {
        const auto xml = Assets::LoadAsXml(path, flags);
        if (!xml.has_value()) {
            return Failure(xml.error());
        }

        const auto root = xml->child("ArraySprite");
        if (!root) {
            return Failure(std::format("Failed to load ArraySprite2D \"{}\": Expected root node \"ArraySprite\"", path));
        }

        const auto texture_node = root.child("Texture");
        if (!texture_node) {
            return Failure(std::format("Failed to load ArraySprite2D \"{}\": Missing required child node \"Texture\"", path));
        }

        const auto texture_path_attrib = texture_node.attribute("src");
        if (!texture_path_attrib) {
            return Failure(std::format("Failed to load ArraySprite2D \"{}\": Missing required attribute \"src\" in child node \"Texture\"", path));
        }

        if (const auto texture_columns_attrib = texture_node.attribute("columns")) {
            if (const auto texture_rows_attrib = texture_node.attribute("rows")) {
                const auto columns = StringToInt<int>(texture_columns_attrib.value());
                const auto rows = StringToInt<int>(texture_rows_attrib.value());
                if (!columns.has_value()) {
                    return Failure(std::format("Failed to load ArraySprite2D \"{}\": Invalid attribute format \"columns\" in child node \"Texture\", \"{}\" is not a valid integer!", path, texture_columns_attrib.value()));
                }
                if (!rows.has_value()) {
                    return Failure(std::format("Failed to load ArraySprite2D \"{}\": Invalid attribute format \"rows\" in child node \"Texture\", \"{}\" is not a valid integer!", path, texture_rows_attrib.value()));
                }

                if (columns.value() <= 0 || rows.value() <= 0) {
                    return Failure(std::format("Failed to load ArraySprite2D \"{}\": Invalid attribute value \"columns\" or \"rows\" in child node \"Texture\", values must be positive, non-zero integers!", path));
                }

                const auto texture = Assets::Load<Texture2D>(texture_path_attrib.value(), flags);
                if (!texture.has_value()) {
                    return Failure(std::format("Failed to load ArraySprite2D \"{}\": Failed to load texture \"{}\": {}", path, texture_path_attrib.value(), texture.error().message));
                }

                return Success<ArraySprite2DPtr>(CreateRef<ArraySprite2D>(
                    texture.value().ptr(), columns.value(), rows.value()
                ));
            }
            return Failure(std::format("Failed to load ArraySprite2D \"{}\": Missing required attribute \"rows\" in child node \"Texture\"", path));
        }
        const auto texture = Assets::Load<Texture2DArray>(texture_path_attrib.value(), flags);
        if (!texture.has_value()) {
            return Failure(std::format("Failed to load ArraySprite2D \"{}\": Failed to load texture array \"{}\"", path, texture_path_attrib.value()));
        }
        return Success<ArraySprite2DPtr>(CreateRef<ArraySprite2D>(texture.value().ptr()));
    }

    void ArraySprite2D::setup_sprite() {
        m_pMaterial->set_opaque(false);
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

    void AnimatedSprite2D::draw_2d(const Rectangle& rect) const {
        ArraySprite2D::draw_2d(rect);
    }

    void AnimatedSprite2D::draw_2d_and_progress_frame(const Rectangle& rect) {
        draw_2d(rect);

        const auto idx = index();
        const float incr = m_fPlaybackSpeed * (m_bBackwards ? -1.0f : 1.0f);

        if (incr == 0.0f) {
            return;
        }

        switch (m_eLoopingType) {
            case SpriteAnimationLoopingType::Loop: {
                if (incr > 0.0f) {
                    if (idx >= max_index()) {
                        set_index(0);
                    }
                } else if (incr < 0.0f) {
                    if (idx <= 0) {
                        set_index(static_cast<int>(max_index() - 1));
                    }
                }
            } break;
            case SpriteAnimationLoopingType::PingPong: {
                if (incr > 0.0f) {
                    if (idx >= max_index()) {
                        m_bBackwards = !m_bBackwards;
                    }
                } else if (incr < 0.0f) {
                    if (idx <= 0) {
                        m_bBackwards = !m_bBackwards;
                    }
                }
            } break;
            case SpriteAnimationLoopingType::PlayOnce: {
                if (incr > 0.0f) {
                    if (idx >= max_index()) {
                        set_playback_speed(0.0f);
                    }
                } else if (incr < 0.0f) {
                    if (idx <= 0) {
                        set_playback_speed(0.0f);
                    }
                }
            } break;
        }

        m_fIndexProgress += incr;
        set_index(static_cast<int>(m_fIndexProgress));
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

    class ThemeResolutionException : public std::runtime_error {
    public:
        explicit ThemeResolutionException(const std::string& message) : std::runtime_error(message) {}
    };

    static String ResolveValue(const String& input, const HashMap<String, String>& constants) {
        if (input.starts_with('$')) {
            const auto key = input.substr(1);
            if (key.starts_with('$')) { // Escape $
                return key;
            }
            if (constants.contains(key)) {
                return constants.at(key);
            }
            throw ThemeResolutionException(std::format("Failed to resolve theme constant \"{}\"", key));
        }
        return input;
    }

    Result<QuadSprite2DPtr> QuadSprite2D::FromXmlWithConstants(const pugi::xml_node& node, const HashMap<String, String>& constants) {
        Color color = ColorConstants::White, border_color = ColorConstants::Black;
        Texture2DPtr texture = nullptr;
        float border_thickness = 0.0f, radius = 0.0f;

        if (const auto cn = node.child("Background"); cn) {
            if (const auto attrib = cn.attribute("color"); attrib) {
                try {
                    const auto value = ResolveValue(attrib.value(), constants);
                    if (auto result = StringToColor(value); !result.has_value()) {
                        Debug::LogError(std::format("Failed to parse color value \"{}\" by attribute \"color\" in node \"Background\": {}, using default value!", value, result.error().message));
                    } else {
                        color = result.value();
                    }
                } catch (const ThemeResolutionException& e) {
                    Debug::LogError(std::format("Failed to resolve theme constant \"{}\" in node \"Background\": {}", e.what(), node.name()));
                }
            }
            if (const auto attrib = cn.attribute("texture"); attrib) {
                try {
                    auto texture_result = Assets::Load<Texture2D>(ResolveValue(attrib.value(), constants));
                    if (!texture_result.has_value()) {
                        return Failure(std::format("Failed to load QuadSprite2D texture: {}", texture_result.error().message));
                    }
                    texture = texture_result.value().ptr();
                } catch (const ThemeResolutionException& e) {
                    Debug::LogError(std::format("Failed to resolve theme constant \"{}\" in node \"Background\": {}", e.what(), node.name()));
                }
            }
        }
        if (const auto cn = node.child("Border"); cn) {
            if (const auto attrib = cn.attribute("color"); attrib) {
                try {
                    const auto value = ResolveValue(attrib.value(), constants);
                    if (auto result = StringToColor(value); !result.has_value()) {
                        Debug::LogError(std::format("Failed to parse color value \"{}\" used by attribute \"color\" in node \"Border\": {}, using default value!", value, result.error().message));
                    } else {
                        border_color = result.value();
                    }
                } catch (const ThemeResolutionException& e) {
                    Debug::LogError(std::format("Failed to resolve theme constant \"{}\" in node \"Border\": {}", e.what(), node.name()));
                }
            }
            if (const auto attrib = cn.attribute("thickness"); attrib) {
                try {
                    const auto value = ResolveValue(attrib.value(), constants);
                    if (auto result = StringToFloat<float>(value); !result.has_value()) {
                        Debug::LogError(std::format("Failed to parse float value \"{}\" used by attribute \"thickness\" in node \"Border\": {}, using default value!", value, result.error().message));
                    } else {
                        border_thickness = result.value();
                    }
                } catch (const ThemeResolutionException& e) {
                    Debug::LogError(std::format("Failed to resolve theme constant \"{}\" in node \"Border\": {}", e.what(), node.name()));
                }
            }
        }
        if (const auto cn = node.child("Radius"); cn) {
            try {
                const auto value = ResolveValue(cn.child_value(), constants);
                if (auto result = StringToFloat<float>(value); !result.has_value()) {
                    Debug::LogError(std::format("Failed to parse float value \"{}\" used by node \"Radius\": {}, using default value!", value, result.error().message));
                } else {
                    radius = result.value();
                }
            } catch (const ThemeResolutionException& e) {
                Debug::LogError(std::format("Failed to resolve theme constant \"{}\" in node \"Radius\": {}", e.what(), node.name()));
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

    LineSprite2D::LineSprite2D(const Texture2DPtr& texture, const Orientation orientation) : m_pTexture(texture), m_eOrientation(orientation) { }
    LineSprite2D::LineSprite2D(Texture2DPtr&& texture, const Orientation orientation) noexcept : m_pTexture(std::move(texture)), m_eOrientation(orientation) { }

    void LineSprite2D::set_texture(const Texture2DPtr& texture) {
        m_pTexture = texture;
        setup_sprite();
    }
    void LineSprite2D::set_texture(Texture2DPtr&& texture) noexcept {
        m_pTexture = std::move(texture);
        setup_sprite();
    }

    void LineSprite2D::set_orientation(Orientation orientation) {
        m_eOrientation = orientation;
        setup_sprite();
    }

    void LineSprite2D::draw_2d(const Rectangle& rect) const {
        if (!is_valid()) {
            return;
        }

        constexpr auto one_third = 1.0f / 3.0f;
        const auto offset = (m_eOrientation == Orientation::Horizontal ? Vector2Constants::UnitX : Vector2Constants::UnitY) * one_third;
        const auto size = rect.size() * offset;

        m_pMeshStart->draw_2d(Rectangle { rect.position(), size });
        if (rect.size().x > m_pTexture->size().x * 2.0f) {
            m_pMeshMiddle->draw_2d(Rectangle { rect.position() + offset, size });
        }
        m_pMeshEnd->draw_2d(Rectangle { rect.width * (2.0f * one_third), 0, rect.width * one_third, rect.height });
    }

    void LineSprite2D::setup_sprite() {
        const auto orientation_vector = m_eOrientation == Orientation::Horizontal ? Vector2Constants::UnitX : Vector2Constants::UnitY;

        if (m_pMeshStart == nullptr) {
            const auto mat = Material::New("StretchSprite2D");
            Debug::Assert(mat);
            if (mat.has_value()) {
                Debug::Assert(mat.value()->set_parameter("MainTexture", m_pTexture));
                Debug::Assert(mat.value()->set_parameter("ImageSize", m_pTexture->size()));
                Debug::Assert(mat.value()->set_parameter("UVStart", Vector2(0.0)));
                Debug::Assert(mat.value()->set_parameter("UVEnd", orientation_vector * 0.25f));
                const auto mesh = Mesh::CreateQuad2D(mat.value());
                Debug::Assert(mesh);
                if (mesh.has_value()) {
                    m_pMeshStart = mesh.value();
                }
            }
        } else {
            Debug::Assert(m_pMeshStart->material()->set_parameter("MainTexture", m_pTexture));
            Debug::Assert(m_pMeshStart->material()->set_parameter("ImageSize", m_pTexture->size()));
            Debug::Assert(m_pMeshStart->material()->set_parameter("UVStart", Vector2(0.0)));
            Debug::Assert(m_pMeshStart->material()->set_parameter("UVEnd", orientation_vector * 0.25f));
        }

        if (m_pMeshMiddle == nullptr) {
            const auto mat = Material::New("StretchSprite2D");
            Debug::Assert(mat);
            if (mat.has_value()) {
                Debug::Assert(mat.value()->set_parameter("MainTexture", m_pTexture));
                Debug::Assert(mat.value()->set_parameter("ImageSize", m_pTexture->size()));
                Debug::Assert(mat.value()->set_parameter("UVStart", orientation_vector * 0.25f));
                Debug::Assert(mat.value()->set_parameter("UVEnd", orientation_vector * 0.5f));
                const auto mesh = Mesh::CreateQuad2D(mat.value(), MeshDrawMode::DynamicDraw);
                Debug::Assert(mesh);
                if (mesh.has_value()) {
                    m_pMeshMiddle = mesh.value();
                }
            }
        } else {
            Debug::Assert(m_pMeshMiddle->material()->set_parameter("MainTexture", m_pTexture));
            Debug::Assert(m_pMeshMiddle->material()->set_parameter("ImageSize", m_pTexture->size()));
            Debug::Assert(m_pMeshMiddle->material()->set_parameter("UVStart", orientation_vector * 0.25f));
            Debug::Assert(m_pMeshMiddle->material()->set_parameter("UVEnd", orientation_vector * 0.5f));
        }

        if (m_pMeshEnd == nullptr) {
            const auto mat = Material::New("StretchSprite2D");
            Debug::Assert(mat);
            if (mat.has_value()) {
                Debug::Assert(mat.value()->set_parameter("MainTexture", m_pTexture));
                Debug::Assert(mat.value()->set_parameter("ImageSize", m_pTexture->size()));
                Debug::Assert(mat.value()->set_parameter("UVStart", orientation_vector * 0.5f));
                Debug::Assert(mat.value()->set_parameter("UVEnd",  orientation_vector * 0.75f));
                const auto mesh = Mesh::CreateQuad2D(mat.value());
                Debug::Assert(mesh);
                if (mesh.has_value()) {
                    m_pMeshEnd = mesh.value();
                }
            }
        } else {
            Debug::Assert(m_pMeshEnd->material()->set_parameter("MainTexture", m_pTexture));
            Debug::Assert(m_pMeshEnd->material()->set_parameter("ImageSize", m_pTexture->size()));
            Debug::Assert(m_pMeshEnd->material()->set_parameter("UVStart", orientation_vector * 0.5f));
            Debug::Assert(m_pMeshEnd->material()->set_parameter("UVEnd",  orientation_vector * 0.75f));
        }
    }

    NineSliceSprite2D::NineSliceSprite2D(const Texture2DPtr& texture) : m_pTexture(texture) {
        setup_sprite();
    }
    NineSliceSprite2D::NineSliceSprite2D(Texture2DPtr&& texture) noexcept : m_pTexture(std::move(texture)) {
        setup_sprite();
    }

    void NineSliceSprite2D::set_texture(const Texture2DPtr& texture) {
        m_pTexture = texture;
        setup_sprite();
    }
    void NineSliceSprite2D::set_texture(Texture2DPtr&& texture) noexcept {
        m_pTexture = std::move(texture);
        setup_sprite();
    }

    void NineSliceSprite2D::draw_2d(const Rectangle& rect) const {
        if (!is_valid()) {
            return;
        }

        constexpr auto one_third = 1.0f / 3.0f;
        Vector2 position = rect.position();

        for (auto i = 0; i < 9; ++i) {
            if (i % 3 == 1 && rect.size().x <= m_pTexture->size().x * 2.0f) {
                continue;
            }

            m_pMeshes[i]->draw_2d(Rectangle { position, rect.size() * one_third });

            position += Vector2(one_third, 0.0) * rect.size();
            if (i % 3 == 2) {
                position.x = 0.0f;
                position.y += one_third * rect.height;
            }
        }
    }

    void NineSliceSprite2D::setup_sprite() {
        auto origin = Vector2(0.0);
        auto end    = Vector2(0.25);

        for (auto i = 0; i < 9; ++i) {
            const auto mat = Material::New("StretchSprite2D");
            Debug::Assert(mat);
            if (mat.has_value()) {
                Debug::Assert(mat.value()->set_parameter("MainTexture", m_pTexture));
                Debug::Assert(mat.value()->set_parameter("ImageSize", m_pTexture->size()));
                Debug::Assert(mat.value()->set_parameter("UVStart", origin));
                Debug::Assert(mat.value()->set_parameter("UVEnd", end));
                const auto mesh = Mesh::CreateQuad2D(mat.value());
                Debug::Assert(mesh);
                if (mesh.has_value()) {
                    m_pMeshes[i] = mesh.value();
                    origin += Vector2(0.25, 0.0);
                    end    += Vector2(0.25, 0.0);
                }
            }

            if (i % 3 == 2) {
                origin.x = 0.0f;
                end.x    = 0.25f;

                origin.y += 0.25f;
                end.y    += 0.25f;
            }
        }
    }

    Font::Font(const Path& path, const uint32_t size) : m_pFace(nullptr) {
        const auto full_path = path.is_absolute() ? path : path.as_absolute(Renderer::GetBasePath() / "res");
        if (!full_path.exists()) {
            Debug::LogError(std::format("Font file \"{}\" does not exist!", path));
            return;
        }

        if (const auto error = FT_New_Face(Renderer::FontLibrary(), full_path.as_cstr(), 0, &m_pFace); error != FT_Err_Ok) {
            const char* error_message = FT_Error_String(error);
            Debug::LogError(std::format("Failed to load font \"{}\": {} ({})", path, error_message != nullptr ? error_message : "Unknown error", static_cast<uint32_t>(error)));
            return;
        }
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
        m_pMaterial->set_parameter("UVOffset", Vector2(0.0));
        m_pMaterial->set_parameter("UVScale",  Vector2(1.0));
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

        BaseTextSprite::setup_sprite();
    }
}
