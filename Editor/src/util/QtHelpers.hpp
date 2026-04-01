#ifndef FOW_EDITOR_QT_HELPERS_HPP
#define FOW_EDITOR_QT_HELPERS_HPP

#include <QWidget>

#include "fow/Shared.hpp"

namespace fow::QtHelpers {
    void ClearLayout(QLayout* layout);
    QColor ColorToQColor(const Color& color);
    Color QColorToColor(const QColor& color);
}

#endif