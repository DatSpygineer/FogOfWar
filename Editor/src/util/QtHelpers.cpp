#include "QtHelpers.hpp"

#include <QLayout>

namespace fow::QtHelpers {
    void ClearLayout(QLayout* layout) {
        if (layout != nullptr) {
            for (auto item = layout->takeAt(0); item != nullptr; item = layout->takeAt(0)) {
                if (item->layout() != nullptr) {
                    ClearLayout(item->layout());
                    delete item->layout();
                }
                if (item->widget() != nullptr) {
                    delete item->widget();
                }
                if (item->spacerItem() != nullptr) {
                    delete item->spacerItem();
                }
            }
        }
    }
    QColor ColorToQColor(const Color& color) {
        return QColor(
            static_cast<uint8_t>(color.r * 255),
            static_cast<uint8_t>(color.g * 255),
            static_cast<uint8_t>(color.b * 255),
            static_cast<uint8_t>(color.a * 255)
        );
    }
    Color QColorToColor(const QColor& color) {
        return Color(
            color.redF(),
            color.greenF(),
            color.blueF(),
            color.alphaF()
        );
    }
}