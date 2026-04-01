#ifndef FOW_EDITOR_IMAGEVIEWER_HPP
#define FOW_EDITOR_IMAGEVIEWER_HPP

#include <QWidget>
#include <QScrollArea>
#include <QImage>
#include <QLabel>

#include "fow/Shared.hpp"

namespace fow {
    class ImageViewer : public QWidget {
        QImage m_image;
        QLabel* m_pLabel;
        QScrollArea* m_pScrollArea;
    public:
        explicit ImageViewer(const QImage& image, QWidget* parent = nullptr);
        explicit ImageViewer(QWidget* parent = nullptr) : ImageViewer(QImage(), parent) { }

        void setImage(const Path& imagePath);
        void setImage(const QImage& image);
        [[nodiscard]] FOW_CONSTEXPR const QImage& getImage() const  { return m_image; }
    private:
        void updateImage() const;
    };
}

#endif