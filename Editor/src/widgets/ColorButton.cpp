#include "ColorButton.hpp"

#include <QColorDialog>

namespace fow {
    ColorButton::ColorButton(const Color& color, QWidget* parent) : QPushButton(parent) {
        setColor(color);
        connect(this, &QPushButton::clicked, this, [this] {
            auto new_color = QColorDialog::getColor(m_color, this);
            if (m_color != new_color) {
                setColor(new_color);
            }
        });
    }

    void ColorButton::setColor(const QColor& color) {
        m_color = color;
        updateColor();
        Q_EMIT colorChanged(m_color);
    }

    void ColorButton::setColor(const Color& color) {
        const auto [ r, g, b, a ] = color.to_bytes();
        setColor(QColor(r, g, b, a));
    }

    void ColorButton::updateColor() {
        setStyleSheet("background-color: " + m_color.name());
    }
}
