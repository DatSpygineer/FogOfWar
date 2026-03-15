#include "QtHelpers.hpp"

#include <QLayout>

namespace fow::QtHelpers {
    void ClearLayout(QLayout* layout) {
        if (layout != nullptr) {
            for (auto item = layout->takeAt(0); item != nullptr; item = layout->takeAt(0)) {
                if (item->layout() != nullptr) {
                    DISCARD(item->layout()->disconnect());
                    ClearLayout(item->layout());
                    delete item->layout();
                }
                if (item->widget() != nullptr) {
                    DISCARD(item->widget()->disconnect());
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