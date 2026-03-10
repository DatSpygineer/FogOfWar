#ifndef FOW_EDITOR_COLOR_BUTTON_HPP
#define FOW_EDITOR_COLOR_BUTTON_HPP

#include <QWidget>
#include <QPushButton>
#include <fow/Shared.hpp>

namespace fow {
    class ColorButton : public QPushButton {
        Q_OBJECT
        QColor m_color;
    public:
        explicit ColorButton(const Color& color, QWidget* parent = nullptr);

        [[nodiscard]] FOW_CONSTEXPR Color color() const { return Color(m_color.redF(), m_color.greenF(), m_color.blueF(), m_color.alphaF()); }
        void setColor(const QColor& color);
        void setColor(const Color& color);

    Q_SIGNALS:
        void colorChanged(const QColor& color);
    private:
        void updateColor();
    };
}

#endif