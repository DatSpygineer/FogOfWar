#include <fow/Renderer/Gui.hpp>

namespace fow::Gui {
    Vector2 Widget::position() const {
        return Vector2(m_area.x, m_area.y);
    }
    Vector2 Widget::size() const {
        return Vector2(m_area.width, m_area.height);
    }

    void Widget::set_area(const Rectangle& area) {
        const Rectangle previous = m_area;
        m_area = area;
        if (m_area.x != previous.x || m_area.y != previous.y) {
            on_moved();
        }
        if (m_area.width != previous.width || m_area.height != previous.height) {
            on_resized();
        }
    }
    void Widget::set_position(const Vector2& position) {
        m_area.x = position.x;
        m_area.y = position.y;
        on_moved();
    }
    void Widget::set_size(const Vector2& size) {
        m_area.x = size.x;
        m_area.y = size.y;
        on_resized();
    }

    void Widget::set_visible(const bool visible) {
        m_bVisible = visible;
        on_visibility_changed(visible);
    }

    void Widget::set_depth(const int depth) {
        m_iDepth = depth;
    }

    void Panel::set_background_color(const Color& color) {
        m_bgColor = color;
        on_background_changed();
    }

    void Panel::draw() {
        
    }
}
