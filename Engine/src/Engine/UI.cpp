#include "fow/Engine/UI.hpp"

#include "fow/Engine.hpp"

namespace fow::UI {
    static HashMap<String, String> s_theme_constants = { };

    class ThemeResolutionException : public std::runtime_error {
    public:
        explicit ThemeResolutionException(const std::string& message) : std::runtime_error(message) {}
    };

    static String ResolveValue(const String& input) {
        if (input.starts_with('$')) {
            auto key = input.substr(1);
            if (key.starts_with('$')) { // Escape $
                return key;
            }
            if (s_theme_constants.contains(key)) {
                return s_theme_constants.at(key);
            }
            throw ThemeResolutionException(std::format("Failed to resolve theme constant \"{}\"", key));
        }
        return input;
    }

    Result<TextTheme> TextTheme::FromXml(const pugi::xml_node& node) {
        const auto font_node = node.child("Font");
        if (!font_node) {
            return Failure(std::format("Expected child node 'Font'"));
        }

        const auto font_path_attrib = font_node.attribute("src");
        if (!font_path_attrib) {
            return Failure(std::format("Expected attribute 'src' in child node 'Font'"));
        }

        const auto font_size_attrib = font_node.attribute("size");
        if (!font_size_attrib) {
            return Failure(std::format("Expected attribute 'size'"));
        }

        FontPtr font;
        try {
            font = CreateRef<Font>(
                ResolveValue(font_path_attrib.value()),
                StringToInt<uint32_t>(ResolveValue(font_size_attrib.value())).value_or(12)
            );
        } catch (const ThemeResolutionException& e) {
            return Failure(e.what());
        }

        Color color, disabled_color;
        if (const auto color_node = node.child("Color")) {
            try {
                color = StringToColor(ResolveValue(color_node.child_value())).value_or(ColorConstants::White);
            } catch (const ThemeResolutionException& e) {
                return Failure(e.what());
            }
        } else {
            color = ColorConstants::White;
        }
        if (const auto color_node = node.child("DisabledColor")) {
            try {
                disabled_color = StringToColor(ResolveValue(color_node.child_value())).value_or(ColorConstants::Gray);
            } catch (const ThemeResolutionException& e) {
                return Failure(e.what());
            }
        } else {
            disabled_color = ColorConstants::Gray;
        }

        TextAlignment alignment = TextAlignment::Default;
        if (const auto text_alignment_node = node.child("Alignment")) {
            const auto vertical = text_alignment_node.attribute("vertical");
            if (!vertical) {
                return Failure(std::format("Expected attribute 'vertical' in child node 'Alignment'"));
            }
            const auto horizontal = text_alignment_node.attribute("horizontal");
            if (!horizontal) {
                return Failure(std::format(" Expected attribute 'horizontal' in child node 'Alignment'"));
            }

            try {
                alignment.vertical = Match<String, VerticalAlignment>(ResolveValue(vertical.value()).clone_lowercase(), VerticalAlignment::Top,
                    HashMap<String, VerticalAlignment> {
                        { "top"_s,    VerticalAlignment::Top    },
                        { "center"_s, VerticalAlignment::Center },
                        { "centre"_s, VerticalAlignment::Center },
                        { "bottom"_s, VerticalAlignment::Bottom }
                    }
                );
                alignment.horizontal = Match<String, HorizontalAlignment>(ResolveValue(horizontal.value()).clone_lowercase(), HorizontalAlignment::Left,
                    HashMap<String, HorizontalAlignment> {
                        { "left"_s,   HorizontalAlignment::Left   },
                        { "center"_s, HorizontalAlignment::Center },
                        { "centre"_s, HorizontalAlignment::Center },
                        { "right"_s,  HorizontalAlignment::Right  }
                    }
                );
            } catch (const ThemeResolutionException& e) {
                return Failure(e.what());
            }
        }

        return Success<TextTheme>(TextTheme {
            font,
            color,
            disabled_color,
            alignment
        });
    }

    Result<RectangleTheme> RectangleTheme::FromXml(const pugi::xml_node& node) {
        if (const auto attrib = node.attribute("type"); attrib) {
            if (String(attrib.value()).equals_any({ "nineslice", "9slice" })) {
                auto bg_node = node.child("Background");
                if (!bg_node) {
                    return Failure(std::format("Expected child node 'Background'"));
                }

                auto bg_texture_attrib = bg_node.attribute("texture");
                if (!bg_texture_attrib) {
                    return Failure(std::format("Expected attribute 'texture' in child node 'Background'"));
                }
                auto bg_texture_path = bg_texture_attrib.value();
                auto bg_texture_result = Assets::Load<Texture2D>(bg_texture_path);
                if (!bg_texture_result.has_value()) {
                    return Failure(std::format("Failed to load texture \"{}\": {}", bg_texture_path, bg_texture_result.error().message));
                }
                auto bg_texture = bg_texture_result.value().ptr();

                return Success<RectangleTheme>(RectangleTheme {
                    CreateRef<NineSliceSprite2D>(bg_texture)
                });
            }
        }

        const auto result = QuadSprite2D::FromXmlWithConstants(node, s_theme_constants);
        if (result.has_value()) {
            return Success<RectangleTheme>(RectangleTheme {
                result.value()
            });
        }
        return Failure(result.error());
    }

    Result<TextboxTheme> TextboxTheme::FromXml(const pugi::xml_node& node) {
        const auto background_node = node.child("Background");
        if (!background_node) {
            return Failure(std::format("Expected child node 'Background'"));
        }

        const auto background_result = QuadSprite2D::FromXmlWithConstants(background_node, s_theme_constants);
        if (!background_result.has_value()) {
            return Failure(std::format("Failed to load background sprite: {}", background_result.error().message));
        }

        const auto foreground_node = node.child("Foreground");
        if (!foreground_node) {
            return Failure(std::format("Expected child node 'Foreground'"));
        }

        const auto foreground_result = TextTheme::FromXml(foreground_node);
        if (!foreground_result.has_value()) {
            return Failure(std::format("Failed to load foreground theme: {}", foreground_result.error().message));
        }

        return Success<TextboxTheme>(TextboxTheme{ background_result.value(), foreground_result.value() });
    }

    Result<ImageTheme> ImageTheme::FromXml(const pugi::xml_node& node) {
        const auto background_node = node.child("Background");
        if (!background_node) {
            return Failure(std::format("Expected child node 'Background'"));
        }

        const auto background_result = QuadSprite2D::FromXmlWithConstants(background_node, s_theme_constants);
        if (!background_result.has_value()) {
            return Failure(std::format("Failed to load background sprite: {}", background_result.error().message));
        }

        const QuadSprite2DPtr& background = background_result.value();
        QuadSprite2DPtr background_disabled;

        if (const auto background_disabled_node = node.child("BackgroundDisabled")) {
            const auto background_disabled_result = QuadSprite2D::FromXmlWithConstants(background_disabled_node, s_theme_constants);
            if (!background_disabled_result.has_value()) {
                return Failure(std::format("Failed to load background disabled sprite: {}", background_disabled_result.error().message));
            }
            background_disabled = background_disabled_result.value();
        } else {
            background_disabled = background;
        }

        return Success<ImageTheme>(ImageTheme {
            background,
            background_disabled
        });
    }

    Result<ButtonTheme> ButtonTheme::FromXml(const pugi::xml_node& node) {
        TextboxTheme normal, selected, pressed, pressed_selected, pressed_disabled, disabled;

        const auto normal_node = node.child("Normal");
        if (!normal_node) {
            return Failure(std::format("Expected child node 'Normal'"));
        }
        const auto normal_result = TextboxTheme::FromXml(normal_node);
        if (!normal_result.has_value()) {
            return Failure(std::format("Failed to load normal theme: {}", normal_result.error().message));
        }
        normal = normal_result.value();

        const auto selected_node = node.child("Selected");
        if (!selected_node) {
            return Failure(std::format("Expected child node 'Selected'"));
        }
        const auto selected_result = TextboxTheme::FromXml(selected_node);
        if (!selected_result.has_value()) {
            return Failure(std::format("Failed to load selected theme: {}", selected_result.error().message));
        }
        selected = selected_result.value();

        const auto pressed_node = node.child("Pressed");
        if (!pressed_node) {
            return Failure(std::format("Expected child node 'Pressed'"));
        }
        const auto pressed_result = TextboxTheme::FromXml(pressed_node);
        if (!pressed_result.has_value()) {
            return Failure(std::format("Failed to load pressed theme: {}", pressed_result.error().message));
        }
        pressed = pressed_result.value();

        const auto disabled_node = node.child("Disabled");
        if (!disabled_node) {
            return Failure(std::format("Expected child node 'Disabled'"));
        }
        const auto disabled_result = TextboxTheme::FromXml(disabled_node);
        if (!disabled_result.has_value()) {
            return Failure(std::format("Failed to load disabled theme: {}", disabled_result.error().message));
        }
        disabled = disabled_result.value();

        const auto pressed_selected_node = node.child("PressedSelected");
        if (!pressed_selected_node) {
            return Failure(std::format("Expected child node 'PressedSelected'"));
        }
        const auto pressed_selected_result = TextboxTheme::FromXml(pressed_selected_node);
        if (!pressed_selected_result.has_value()) {
            return Failure(std::format("Failed to load pressed selected theme: {}", pressed_selected_result.error().message));
        }
        pressed_selected = pressed_selected_result.value();

        const auto pressed_disabled_node = node.child("PressedDisabled");
        if (!pressed_disabled_node) {
            return Failure(std::format("Expected child node 'PressedDisabled'"));
        }
        const auto pressed_disabled_result = TextboxTheme::FromXml(pressed_disabled_node);
        if (!pressed_disabled_result.has_value()) {
            return Failure(std::format("Failed to load pressed disabled theme: {}", pressed_disabled_result.error().message));
        }
        pressed_disabled = pressed_disabled_result.value();

        return Success<ButtonTheme>(ButtonTheme {
            normal, selected,
            pressed, pressed_selected,
            pressed_disabled,
            disabled
        });
    }

    Result<CheckBoxTheme> CheckBoxTheme::FromXml(const pugi::xml_node& node) {
        const auto sprite_node = node.child("Sprite");
        if (!sprite_node) {
            return Failure(std::format("Expected child node 'Sprite'"));
        }
        const auto sprite_result = Assets::Load<ArraySprite2D>(sprite_node.child_value());
        if (!sprite_result.has_value()) {
            return Failure(std::format("Failed to load sprite: {}", sprite_result.error().message));
        }


        const auto foreground_node = node.child("Foreground");
        if (!foreground_node) {
            return Failure(std::format("Expected child node 'Foreground'"));
        }

        const auto foreground_result = TextTheme::FromXml(foreground_node);
        if (!foreground_result.has_value()) {
            return Failure(std::format("Failed to load foreground text theme: {}", foreground_result.error().message));
        }

        return Success<CheckBoxTheme>(CheckBoxTheme {
            sprite_result.value().ptr(),
            foreground_result.value()
        });
    }

    Result<ThemePtr> Theme::FromXml(const pugi::xml_node& node) {
        s_theme_constants.clear();

        if (const auto constants_node = node.child("Constants")) {
            for (const auto& child : constants_node.children()) {
                const String name = child.name();
                const String value = child.child_value();
                if (s_theme_constants.contains(name)) {
                    Debug::LogWarning(std::format("Constant \"{}\" in theme is defined multiple times, previous value will be overwritten!", name));
                    s_theme_constants.at(name) = value;
                } else {
                    s_theme_constants.emplace(name, value);
                }
            }
        }

        const auto panel_node = node.child("Panel");
        if (!panel_node) {
            return Failure(std::format("Expected child node 'Panel'"));
        }
        const auto panel_result = RectangleTheme::FromXml(panel_node);
        if (!panel_result.has_value()) {
            return Failure(std::format("Failed to load panel theme: {}", panel_result.error().message));
        }
        const auto& panel = panel_result.value();

        const auto label_node = node.child("Label");
        if (!label_node) {
            return Failure(std::format("Expected child node 'Label'"));
        }
        const auto label_result = TextTheme::FromXml(label_node);
        if (!label_result.has_value()) {
            return Failure(std::format("Failed to load label theme: {}", label_result.error().message));
        }
        const auto& label = label_result.value();

        const auto textbox_node = node.child("TextBox");
        if (!textbox_node) {
            return Failure(std::format("Expected child node 'TextBox'"));
        }
        const auto textbox_result = TextboxTheme::FromXml(textbox_node);
        if (!textbox_result.has_value()) {
            return Failure(std::format("Failed to load textbox theme: {}", textbox_result.error().message));
        }
        const auto& textbox = textbox_result.value();

        const auto image_node = node.child("Image");
        if (!image_node) {
            return Failure(std::format("Expected child node 'Image'"));
        }
        const auto image_result = ImageTheme::FromXml(image_node);
        if (!image_result.has_value()) {
            return Failure(std::format("Failed to load image theme: {}", image_result.error().message));
        }
        const auto& image = image_result.value();

        const auto button_node = node.child("Button");
        if (!button_node) {
            return Failure(std::format("Expected child node 'Button'"));
        }
        const auto button_result = ButtonTheme::FromXml(button_node);
        if (!button_result.has_value()) {
            return Failure(std::format("Failed to load button theme: {}", button_result.error().message));
        }
        const auto& button = button_result.value();

        const auto checkbox_node = node.child("CheckBox");
        if (!checkbox_node) {
            return Failure(std::format("Expected child node 'CheckBox'"));
        }
        const auto checkbox_result = CheckBoxTheme::FromXml(checkbox_node);
        if (!checkbox_result.has_value()) {
            return Failure(std::format("Failed to load checkbox theme: {}", checkbox_result.error().message));
        }
        const auto& checkbox = checkbox_result.value();

        return Success(CreateRef<Theme>(panel, label, textbox, image, button, checkbox));
    }

    Result<ThemePtr> Theme::LoadAsset(const Path& path, const AssetLoaderFlags::Type flags) {
        const auto xml = Assets::LoadAsXml(path, flags);
        if (xml.has_value()) {
            if (const auto root = xml->child("Theme")) {
                return FromXml(root);
            }
            return Failure(std::format("Failed to load theme \"{}\": Expected root node \"Theme\"", path));
        }
        return Failure(std::format("Failed to load theme \"{}\": {}", path, xml.error().message));
    }

    void Frame::add_widget(const WidgetPtr& widget) {
        m_Widgets.emplace_back(widget);
    }
    void Frame::remove_widget(const WidgetPtr& widget) {
        if (const auto& it = std::ranges::find(m_Widgets, widget); it != m_Widgets.end()) {
            m_Widgets.erase(it);
        }
    }

    void Frame::update(const double dt) const {
        for (const auto& widget : m_Widgets) {
            widget->on_update(dt);
        }
    }
    void Frame::render() const {
        for (const auto& widget : m_Widgets) {
            widget->on_draw();
        }
    }

    void Widget::on_update(const double dt) {
        if (!m_bMouseInside) {
            if (m_Area.is_point_inside(Input::MousePosition())) {
                m_bMouseInside = true;
                on_mouse_entered();
            }
        } else {
            if (!m_Area.is_point_inside(Input::MousePosition())) {
                m_bMouseInside = false;
                on_mouse_left();
            }
        }
    }

    void Widget::set_area(const IntRectangle& area) {
        const auto prev = m_Area;
        m_Area = area;
        if (prev.size() != m_Area.size()) {
            on_resized();
        }
        if (prev.position() != m_Area.position()) {
            on_moved();
        }
    }

    void Widget::set_visible(const bool visible) {
        if (visible == m_bVisible) {
            return;
        }
        m_bVisible = visible;
        on_visibility_changed(visible);
    }

    void Widget::set_enabled(const bool enabled) {
        if (enabled == m_bEnabled) {
            return;
        }
        m_bEnabled = enabled;
        on_enabled_changed(enabled);
    }

    void Panel::set_theme(const RectangleTheme& theme) {
        m_Theme = theme;
    }

    void Panel::on_draw() {
        Ref<IDrawable2D> sprite;
        if (m_Theme.sprite.index() == 0) {
            sprite = std::get<0>(m_Theme.sprite);
        } else {
            sprite = std::get<1>(m_Theme.sprite);
        }

        if (sprite != nullptr && is_visible() && is_enabled()) {
            sprite->draw_2d(area());
        }
    }

    Label::Label(const FramePtr& frame, const String& text) : Label(frame, text, frame->theme()->Text) { }
    Label::Label(const FramePtr& frame, const String& text, const TextTheme& theme) : Widget(frame), m_Theme(theme), m_pText(nullptr), m_sText(text) {
        auto material = Material::New("Generic2D");
        Debug::AssertFatal(material);
        if (material.has_value()) {
            material->get()->set_opaque(false);
            m_pText = CreateRef<TextSprite2D>(m_sText, m_Theme.font, material.value(), area().without_offset());
            m_pText->set_color(m_Theme.color);
            m_pText->set_alignment(m_Theme.alignment);
        }
    }
    Label::Label(const FramePtr& frame, const String& text, TextTheme&& theme) : Widget(frame), m_Theme(std::move(theme)), m_pText(nullptr), m_sText(text) {
        auto material = Material::New("Generic2D");
        Debug::AssertFatal(material);
        if (material.has_value()) {
            material->get()->set_opaque(false);
            m_pText = CreateRef<TextSprite2D>(m_sText, m_Theme.font, material.value(), area().without_offset());
            m_pText->set_color(m_Theme.color);
            m_pText->set_alignment(m_Theme.alignment);
        }
    }
    Label::Label(const FramePtr& frame, String&& text) : Label(frame, std::move(text), frame->theme()->Text) { }
    Label::Label(const FramePtr& frame, String&& text, const TextTheme& theme) : Widget(frame), m_Theme(theme), m_pText(nullptr), m_sText(std::move(text)) {
        auto material = Material::New("Generic2D");
        Debug::AssertFatal(material);
        if (material.has_value()) {
            material->get()->set_opaque(false);
            m_pText = CreateRef<TextSprite2D>(m_sText, m_Theme.font, material.value(), area().without_offset());
            m_pText->set_color(m_Theme.color);
            m_pText->set_alignment(m_Theme.alignment);
        }
    }
    Label::Label(const FramePtr& frame, String&& text, TextTheme&& theme) : Widget(frame), m_Theme(std::move(theme)), m_pText(nullptr), m_sText(std::move(text)) {
        auto material = Material::New("Generic2D");
        Debug::AssertFatal(material);
        if (material.has_value()) {
            material->get()->set_opaque(false);
            m_pText = CreateRef<TextSprite2D>(m_sText, m_Theme.font, material.value(), area().without_offset());
            m_pText->set_color(m_Theme.color);
            m_pText->set_alignment(m_Theme.alignment);
        }
    }

    void Label::set_theme(const TextTheme& theme) {
        m_Theme = theme;
        m_pText->set_font(m_Theme.font);
        m_pText->set_color(m_Theme.color);
        m_pText->set_alignment(m_Theme.alignment);
    }

    void Label::set_text(const String& text) {
        m_sText = text;
        m_pText->set_text(text);
    }

    void Label::on_resized() {
        m_pText->set_text_area(area().without_offset());
    }

    void Label::on_draw() {
        if (m_pText != nullptr && is_visible()) {
            m_pText->draw_2d(area());
        }
    }

    void Label::on_enabled_changed(const bool enabled) {
        Widget::on_enabled_changed(enabled);
        m_pText->set_color(enabled ? m_Theme.color : m_Theme.disabled_color);
    }

    void ImageSprite::set_theme(const ImageTheme& theme) {
        m_Theme = theme;
    }

    void ImageSprite::set_image(const Texture2DPtr& normal, const Texture2DPtr& disabled) const {
        m_Theme.background->set_background_texture(normal);
        m_Theme.background_disabled->set_background_texture(disabled);
    }

    void ImageSprite::on_draw() {
        if (is_visible()) {
            if (is_enabled()) {
                m_Theme.background->draw_2d(area());
            } else {
                m_Theme.background_disabled->draw_2d(area());
            }
        }
    }

    void BaseButton::on_update(const double dt) {
        Widget::on_update(dt);

        if (is_enabled() && is_visible()) {
            if (!m_bToggle) {
                if (!m_bPressed) {
                    if (is_mouse_over() && Input::MouseIsPressed(Input::MouseButton::Left)) {
                        m_bPressed = true;
                        on_pressed();
                    }
                } else if (Input::MouseIsUp(Input::MouseButton::Left)) {
                    on_clicked();
                    m_bPressed = false;
                    on_released();
                }
            } else {
                if (is_mouse_over() && Input::MouseIsPressed(Input::MouseButton::Left)) {
                    m_bPressed = !m_bPressed;
                    on_clicked();
                }
            }
        }
    }

    void BaseButton::on_clicked() {
        if (m_fnClickedCallback) {
            m_fnClickedCallback();
        }
    }

    Button::Button(const FramePtr& frame, const String& text, const ButtonTheme& theme) : BaseButton(frame), m_Theme(theme), m_sText(text), m_pText(nullptr), m_bSelected(false) {
        auto material = Material::New("Generic2D");
        Debug::AssertFatal(material);
        if (material.has_value()) {
            material->get()->set_opaque(false);
            material->get()->set_parameter("DEPTH", 1);
            const auto& [ font, color, _, alignment ] = theme.normal.foreground;
            m_pText = CreateRef<TextSprite2D>(m_sText, font, material.value(), area().without_offset());
            m_pText->set_color(color);
            m_pText->set_alignment(alignment);
        }
    }
    Button::Button(const FramePtr& frame, const String& text) : Button(frame, text, frame->theme()->Button) { }
    Button::Button(const FramePtr& frame, String&& text, const ButtonTheme& theme) : BaseButton(frame), m_Theme(theme), m_sText(std::move(text)), m_pText(nullptr), m_bSelected(false) {
        auto material = Material::New("Generic2D");
        Debug::AssertFatal(material);
        if (material.has_value()) {
            material->get()->set_opaque(false);
            material->get()->set_parameter("DEPTH", 1);
            const auto& [ font, color, _, alignment ] = theme.normal.foreground;
            m_pText = CreateRef<TextSprite2D>(m_sText, font, material.value(), area().without_offset());
            m_pText->set_color(color);
            m_pText->set_alignment(alignment);
        }
    }
    Button::Button(const FramePtr& frame, String&& text, ButtonTheme&& theme) : BaseButton(frame), m_Theme(std::move(theme)), m_sText(std::move(text)), m_pText(nullptr), m_bSelected(false) {
        auto material = Material::New("Generic2D");
        Debug::AssertFatal(material);
        if (material.has_value()) {
            material->get()->set_opaque(false);
            material->get()->set_parameter("DEPTH", 1);
            const auto& [ font, color, _, alignment ] = theme.normal.foreground;
            m_pText = CreateRef<TextSprite2D>(m_sText, font, material.value(), area().without_offset());
            m_pText->set_color(color);
            m_pText->set_alignment(alignment);
        }
    }
    Button::Button(const FramePtr& frame, String&& text) : Button(frame, std::move(text), frame->theme()->Button) { }

    void Button::set_theme(const ButtonTheme& theme) {
        m_Theme = theme;
    }

    void Button::set_text(const String& text) {
        m_sText = text;
        m_pText->set_text(text);
        update_text_theme();
    }

    void Button::set_toggle(const bool is_toggle) {
        m_bToggle = is_toggle;
        m_bPressed = false;
        update_text_theme();
    }

    void Button::on_update(const double dt) {
        BaseButton::on_update(dt);
    }

    void Button::on_draw() {
        if (!is_visible()) {
            return;
        }

        Ref<IDrawable2D> sprite = nullptr;
        if (m_bPressed) {
            if (is_enabled()) {
                if (m_bSelected) {
                    if (m_Theme.pressed_selected.background.sprite.index() == 0) {
                        sprite = std::get<0>(m_Theme.pressed_selected.background.sprite);
                    } else {
                        sprite = std::get<1>(m_Theme.pressed_selected.background.sprite);
                    }
                } else {
                    if (m_Theme.pressed.background.sprite.index() == 0) {
                        sprite = std::get<0>(m_Theme.pressed.background.sprite);
                    } else {
                        sprite = std::get<1>(m_Theme.pressed.background.sprite);
                    }
                }
            } else {
                if (m_Theme.pressed_disabled.background.sprite.index() == 0) {
                    sprite = std::get<0>(m_Theme.pressed_disabled.background.sprite);
                } else {
                    sprite = std::get<1>(m_Theme.pressed_disabled.background.sprite);
                }
            }
        } else {
            if (is_enabled()) {
                if (m_bSelected) {
                    if (m_Theme.selected.background.sprite.index() == 0) {
                        sprite = std::get<0>(m_Theme.selected.background.sprite);
                    } else {
                        sprite = std::get<1>(m_Theme.selected.background.sprite);
                    }
                } else {
                    if (m_Theme.normal.background.sprite.index() == 0) {
                        sprite = std::get<0>(m_Theme.normal.background.sprite);
                    } else {
                        sprite = std::get<1>(m_Theme.normal.background.sprite);
                    }
                }
            } else {
                if (m_Theme.disabled.background.sprite.index() == 0) {
                    sprite = std::get<0>(m_Theme.disabled.background.sprite);
                } else {
                    sprite = std::get<1>(m_Theme.disabled.background.sprite);
                }
            }
        }

        if (sprite != nullptr) {
            sprite->draw_2d(area());
        }
        if (m_pText != nullptr) {
            m_pText->draw_2d(area());
        }
    }

    void Button::on_clicked() {
        BaseButton::on_clicked();
        update_text_theme();
    }

    void Button::on_pressed() {
        update_text_theme();
    }
    void Button::on_released() {
        update_text_theme();
    }

    void Button::on_mouse_entered() {
        m_bSelected = true;
        update_text_theme();
    }
    void Button::on_mouse_left() {
        m_bSelected = false;
        update_text_theme();
    }

    void Button::on_resized() {
        Widget::on_resized();
        m_pText->set_text_area(area().without_offset());
    }

    void Button::on_enabled_changed(const bool enabled) {
        Widget::on_enabled_changed(enabled);
        update_text_theme();
    }

    void Button::update_text_theme() const {
        if (m_bPressed) {
            if (is_enabled()) {
                if (m_bSelected) {
                    m_pText->set_font(m_Theme.pressed_selected.foreground.font);
                    m_pText->set_color(m_Theme.pressed_selected.foreground.color);
                    m_pText->set_alignment(m_Theme.pressed_selected.foreground.alignment);
                } else {
                    m_pText->set_font(m_Theme.pressed.foreground.font);
                    m_pText->set_color(m_Theme.pressed.foreground.color);
                    m_pText->set_alignment(m_Theme.pressed.foreground.alignment);
                }
            } else {
                m_pText->set_font(m_Theme.pressed_disabled.foreground.font);
                m_pText->set_color(m_Theme.pressed_disabled.foreground.color);
                m_pText->set_alignment(m_Theme.pressed_disabled.foreground.alignment);
            }
        } else {
            if (is_enabled()) {
                if (m_bSelected) {
                    m_pText->set_font(m_Theme.selected.foreground.font);
                    m_pText->set_color(m_Theme.selected.foreground.color);
                    m_pText->set_alignment(m_Theme.selected.foreground.alignment);
                } else {
                    m_pText->set_font(m_Theme.normal.foreground.font);
                    m_pText->set_color(m_Theme.normal.foreground.color);
                    m_pText->set_alignment(m_Theme.normal.foreground.alignment);
                }
            } else {
                m_pText->set_font(m_Theme.disabled.foreground.font);
                m_pText->set_color(m_Theme.disabled.foreground.color);
                m_pText->set_alignment(m_Theme.disabled.foreground.alignment);
            }
        }
    }

#define CHECKBOX_SPRITE_INDEX_NORMAL           0
#define CHECKBOX_SPRITE_INDEX_CHECKED          1
#define CHECKBOX_SPRITE_INDEX_SELECTED         2
#define CHECKBOX_SPRITE_INDEX_SELECTED_CHECKED 3
#define CHECKBOX_SPRITE_INDEX_DISABLED         4
#define CHECKBOX_SPRITE_INDEX_DISABLED_CHECKED 5

    void CheckBox::on_clicked() {
        BaseButton::on_clicked();
        m_bChecked = !m_bChecked;
        update_sprite();
    }

    void CheckBox::on_update(const double dt) {
        BaseButton::on_update(dt);
    }

    void CheckBox::on_draw() {
        BaseButton::on_draw();
        if (m_Theme.sprite_sheet != nullptr) {
            m_Theme.sprite_sheet->draw_2d(area());
        }
    }

    void CheckBox::on_mouse_entered() {
        BaseButton::on_mouse_entered();
        m_bSelected = true;
        update_sprite();
    }
    void CheckBox::on_mouse_left() {
        BaseButton::on_mouse_left();
        m_bSelected = false;
        update_sprite();
    }

    void CheckBox::update_sprite() const {
        if (m_Theme.sprite_sheet != nullptr) {
            if (m_bSelected) {
                if (m_bChecked) {
                    m_Theme.sprite_sheet->set_index(is_enabled() ? CHECKBOX_SPRITE_INDEX_SELECTED_CHECKED : CHECKBOX_SPRITE_INDEX_DISABLED_CHECKED);
                } else {
                    m_Theme.sprite_sheet->set_index(is_enabled() ? CHECKBOX_SPRITE_INDEX_SELECTED : CHECKBOX_SPRITE_INDEX_DISABLED);
                }
            } else {
                if (m_bChecked) {
                    m_Theme.sprite_sheet->set_index(is_enabled() ? CHECKBOX_SPRITE_INDEX_CHECKED : CHECKBOX_SPRITE_INDEX_DISABLED_CHECKED);
                } else {
                    m_Theme.sprite_sheet->set_index(is_enabled() ? CHECKBOX_SPRITE_INDEX_NORMAL: CHECKBOX_SPRITE_INDEX_DISABLED);
                }
            }
        }
    }
}
