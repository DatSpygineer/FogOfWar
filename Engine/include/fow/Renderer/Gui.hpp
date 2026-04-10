#ifndef FOW_RENDERER_GUI_HPP
#define FOW_RENDERER_GUI_HPP

#include "Mesh.hpp"
#include "fow/Shared.hpp"

namespace fow::Gui {
    class Container;
    class Widget;
    using WidgetPtr = Ref<Widget>;

    class FOW_RENDER_API Container {
    public:
        
    };

    class FOW_RENDER_API Widget {
        Rectangle m_area;
        int m_iDepth = 0;
        bool m_bVisible = true;
    public:
        Widget() = default;
        Widget(const Widget& other) = default;
        Widget(Widget&& other) noexcept = default;
        virtual ~Widget() = default;

        [[nodiscard]] FOW_CONSTEXPR const Rectangle& area() const { return m_area; }
        [[nodiscard]] Vector2 position() const;
        [[nodiscard]] Vector2 size() const;
        [[nodiscard]] FOW_CONSTEXPR bool visible() const { return m_bVisible; }
        [[nodiscard]] FOW_CONSTEXPR int depth() const { return m_iDepth; }

        void set_area(const Rectangle& area);
        void set_position(const Vector2& position);
        void set_size(const Vector2& size);
        void set_visible(bool visible);
        void set_depth(int depth);

        virtual void draw() { }

        virtual void on_visibility_changed(bool visible) { }
        virtual void on_moved() { }
        virtual void on_resized() { }
    };

    class FOW_RENDER_API Panel : public Widget {
        Color m_bgColor;
    public:
        Panel() = default;
        Panel(const Panel& other) = default;
        Panel(Panel&& other) noexcept = default;
        ~Panel() override = default;

        [[nodiscard]] FOW_CONSTEXPR const Color& background_color() const { return m_bgColor; }
        void set_background_color(const Color& color);

        void draw() override;

        virtual void on_background_changed() { }
    };
}

#endif