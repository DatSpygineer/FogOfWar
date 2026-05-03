#ifndef FOW_ENGINE_UI_HPP
#define FOW_ENGINE_UI_HPP

#include <fow/Shared.hpp>

#include "UI.hpp"
#include "fow/Renderer/Sprite.hpp"

namespace fow::UI {
    class Frame;
    using FramePtr = Ref<Frame>;
    class Widget;
    using WidgetPtr = Ref<Widget>;

    struct TextTheme;
    struct RectangleTheme;
    struct TextboxTheme;
    struct ImageTheme;
    struct ButtonTheme;
    struct Theme;
    using ThemePtr = Ref<Theme>;

    struct FOW_ENGINE_API TextTheme {
        FontPtr font;
        Color color;
        Color disabled_color;
        TextAlignment alignment;

        static Result<TextTheme> FromXml(const pugi::xml_node& node);
    };
    struct FOW_ENGINE_API RectangleTheme {
        std::variant<QuadSprite2DPtr, NineSliceSprite2DPtr> sprite;

        static Result<RectangleTheme> FromXml(const pugi::xml_node& node);
    };
    struct FOW_ENGINE_API TextboxTheme {
        RectangleTheme background;
        TextTheme foreground;

        static Result<TextboxTheme> FromXml(const pugi::xml_node& node);
    };
    struct FOW_ENGINE_API ImageTheme {
        QuadSprite2DPtr background;
        QuadSprite2DPtr background_disabled;

        static Result<ImageTheme> FromXml(const pugi::xml_node& node);
    };
    struct FOW_ENGINE_API ButtonTheme {
        TextboxTheme normal;
        TextboxTheme selected;
        TextboxTheme pressed;
        TextboxTheme pressed_selected;
        TextboxTheme pressed_disabled;
        TextboxTheme disabled;

        static Result<ButtonTheme> FromXml(const pugi::xml_node& node);
    };
    struct FOW_ENGINE_API CheckBoxTheme {
        ArraySprite2DPtr sprite_sheet;
        TextTheme foreground;

        static Result<CheckBoxTheme> FromXml(const pugi::xml_node& node);
    };

    struct FOW_ENGINE_API Theme {
        RectangleTheme Rectangle;
        TextTheme Text;
        TextboxTheme TextBox;
        ImageTheme Image;
        ButtonTheme Button;
        CheckBoxTheme CheckBox;

        static Result<ThemePtr> FromXml(const pugi::xml_node& node);
        static Result<ThemePtr> LoadAsset(const Path& path, AssetLoaderFlags::Type flags);
    };

    class FOW_ENGINE_API Frame {
        Vector<WidgetPtr> m_Widgets;
        ThemePtr m_pTheme;
    public:
        explicit Frame(const ThemePtr& theme) : m_pTheme(theme) { }
        explicit Frame(ThemePtr&& theme) : m_pTheme(std::move(theme)) { }

        [[nodiscard]] FOW_CONSTEXPR const ThemePtr& theme() const { return m_pTheme; }

        void add_widget(const WidgetPtr& widget);
        void remove_widget(const WidgetPtr& widget);

        void update(double dt) const;
        void render() const;
    };

    class FOW_ENGINE_API Widget {
        FramePtr m_pFrame;
        IntRectangle m_Area = { 0, 0, 64, 64 };
        bool m_bVisible = true;
        bool m_bEnabled = true;
        bool m_bMouseInside = false;
    public:
        virtual ~Widget() = default;

        explicit Widget(const FramePtr& frame) : m_pFrame(frame) { }
        explicit Widget(FramePtr&& frame) : m_pFrame(std::move(frame)) { }

        virtual void on_setup() { }
        virtual void on_update(double dt);
        virtual void on_draw() { }
        virtual void on_destroyed() { }

        virtual void on_resized() { }
        virtual void on_moved() { }
        virtual void on_visibility_changed(bool visible) { }
        virtual void on_enabled_changed(bool enabled) { }

        virtual void on_mouse_entered() { }
        virtual void on_mouse_left() { }

        void set_area(const IntRectangle& area);
        [[nodiscard]] FOW_CONSTEXPR const IntRectangle& area() const { return m_Area; }
        void set_visible(bool visible);
        [[nodiscard]] FOW_CONSTEXPR bool is_visible() const { return m_bVisible; }
        void set_enabled(bool enabled);
        [[nodiscard]] FOW_CONSTEXPR bool is_enabled() const { return m_bEnabled; }
        [[nodiscard]] FOW_CONSTEXPR bool is_mouse_over() const { return m_bMouseInside; }

        [[nodiscard]] FOW_CONSTEXPR const FramePtr& frame() const { return m_pFrame; }
    };

    class FOW_ENGINE_API Panel : public Widget {
        RectangleTheme m_Theme;
    public:
        explicit Panel(const FramePtr& frame) : Widget(frame), m_Theme(frame->theme()->Rectangle) { }
        Panel(const FramePtr& frame, const RectangleTheme& theme) : Widget(frame), m_Theme(theme) { }
        Panel(const FramePtr& frame, RectangleTheme&& theme) : Widget(frame), m_Theme(std::move(theme)) { }

        void set_theme(const RectangleTheme& theme);
        [[nodiscard]] FOW_CONSTEXPR const RectangleTheme& theme() const { return m_Theme; }

        void on_draw() override;
    };

    class FOW_ENGINE_API Label : public Widget {
        TextTheme m_Theme;
        TextSprite2DPtr m_pText;
        String m_sText;
    public:
        Label(const FramePtr& frame, const String& text);
        Label(const FramePtr& frame, const String& text, const TextTheme& theme);
        Label(const FramePtr& frame, const String& text, TextTheme&& theme);
        Label(const FramePtr& frame, String&& text);
        Label(const FramePtr& frame, String&& text, const TextTheme& theme);
        Label(const FramePtr& frame, String&& text, TextTheme&& theme);

        void set_theme(const TextTheme& theme);
        [[nodiscard]] FOW_CONSTEXPR const TextTheme& theme() const { return m_Theme; }

        void set_text(const String& text);
        [[nodiscard]] FOW_CONSTEXPR const String& text() const { return m_sText; }

        void on_resized() override;

        void on_draw() override;

        void on_enabled_changed(bool enabled) override;
    };
    class FOW_ENGINE_API ImageSprite : public Widget {
        ImageTheme m_Theme;
    public:
        ImageSprite(const FramePtr& frame, const ImageTheme& theme) : Widget(frame), m_Theme(theme) { }
        ImageSprite(const FramePtr& frame, ImageTheme&& theme) : Widget(frame), m_Theme(std::move(theme)) { }
        explicit ImageSprite(const FramePtr& frame) : Widget(frame), m_Theme(frame->theme()->Image) { }

        void set_theme(const ImageTheme& theme);
        [[nodiscard]] FOW_CONSTEXPR const ImageTheme& theme() const { return m_Theme; }

        void set_image(const Texture2DPtr& normal, const Texture2DPtr& disabled) const;

        void on_draw() override;
    };

    class FOW_ENGINE_API BaseButton : public Widget {
    protected:
        bool m_bPressed = false;
        bool m_bToggle = false;
        std::function<void()> m_fnClickedCallback = nullptr;
    public:
        explicit BaseButton(const FramePtr& frame) : Widget(frame) { }

        void on_update(double dt) override;

        FOW_ABSTRACT(bool is_toggle() const);

        virtual void set_pressed(const bool value) { m_bPressed = value; }

        virtual void on_clicked();
        virtual void on_pressed() { }
        virtual void on_released() { }

        void set_clicked_callback(const std::function<void()>& callback) { m_fnClickedCallback = callback; }
    };

    class FOW_ENGINE_API Button : public BaseButton {
        ButtonTheme m_Theme;
        String m_sText;
        TextSprite2DPtr m_pText;
        bool m_bSelected;
    public:
        Button(const FramePtr& frame, const String& text, const ButtonTheme& theme);
        Button(const FramePtr& frame, const String& text);
        Button(const FramePtr& frame, String&& text, const ButtonTheme& theme);
        Button(const FramePtr& frame, String&& text, ButtonTheme&& theme);
        Button(const FramePtr& frame, String&& text);

        void set_theme(const ButtonTheme& theme);
        [[nodiscard]] FOW_CONSTEXPR const ButtonTheme& theme() const { return m_Theme; }

        void set_text(const String& text);
        [[nodiscard]] FOW_CONSTEXPR const String& text() const { return m_sText; }

        void set_toggle(bool is_toggle);
        [[nodiscard]] FOW_CONSTEXPR bool is_toggle() const override { return m_bToggle; }

        [[nodiscard]] FOW_CONSTEXPR bool is_pressed() const { return m_bPressed; }

        void on_update(double dt) override;
        void on_draw() override;
        void on_clicked() override;
        void on_pressed() override;
        void on_released() override;

        void on_mouse_entered() override;
        void on_mouse_left() override;

        void on_resized() override;

        void on_enabled_changed(bool enabled) override;
    private:
        void update_text_theme() const;
    };

    class FOW_ENGINE_API CheckBox : public BaseButton {
        CheckBoxTheme m_Theme;
        bool m_bChecked;
        bool m_bSelected;
    public:
        CheckBox(const FramePtr& frame, const CheckBoxTheme& theme) : BaseButton(frame), m_Theme(theme), m_bChecked(false), m_bSelected(false) { }
        CheckBox(const FramePtr& frame, CheckBoxTheme&& theme) : BaseButton(frame), m_Theme(std::move(theme)), m_bChecked(false), m_bSelected(false) { }
        explicit CheckBox(const FramePtr& frame) : BaseButton(frame), m_Theme(frame->theme()->CheckBox), m_bChecked(false), m_bSelected(false) { }

        [[nodiscard]] FOW_CONSTEXPR const CheckBoxTheme& theme() const { return m_Theme; }
        void set_theme(const CheckBoxTheme& theme) { m_Theme = theme; }

        void set_checked(const bool checked) { m_bChecked = checked; }
        [[nodiscard]] FOW_CONSTEXPR bool is_checked() const { return m_bChecked; }
        [[nodiscard]] FOW_CONSTEXPR bool is_toggle() const override { return true; }

        void on_clicked() override;
        void on_update(double dt) override;
        void on_draw() override;

        void on_mouse_entered() override;
        void on_mouse_left() override;
    private:
        void update_sprite() const;
    };

    class Slider;

    class FOW_ENGINE_API SliderHandle : public BaseButton {
        Slider& m_parent;
    public:
        explicit SliderHandle(const FramePtr& frame, Slider& parent) : BaseButton(frame), m_parent(parent) { }

        FOW_CONSTEXPR bool is_toggle() const override { return false; }

        friend class Slider;
    };

    class FOW_ENGINE_API Slider : public Widget {
        float m_fSliderPercentage = 0.0f, m_fMinValue, m_fMaxValue, m_fStep;
        SliderHandle m_Handle;
    public:
        Slider(const FramePtr& frame, const float min_value, const float max_value, const float step = 0.1f) :
            Widget(frame), m_fMinValue(min_value), m_fMaxValue(max_value), m_fStep(step), m_Handle(frame, *this) { }

        [[nodiscard]] FOW_CONSTEXPR float value() const { return Lerp(m_fMinValue, m_fMaxValue, m_fSliderPercentage); }
        [[nodiscard]] FOW_CONSTEXPR float min_value() const { return m_fMinValue; }
        [[nodiscard]] FOW_CONSTEXPR float max_value() const { return m_fMaxValue; }
        [[nodiscard]] FOW_CONSTEXPR float step() const { return m_fStep; }

        void set_value(float value);
        void set_min_value(float value);
        void set_max_value(float value);
        void set_step(float value);
    };
}

#endif