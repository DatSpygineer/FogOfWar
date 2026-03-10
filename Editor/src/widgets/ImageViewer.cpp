#include "ImageViewer.hpp"

#include <QImageReader>
#include <QVBoxLayout>

namespace fow {
    ImageViewer::ImageViewer(const QImage& image, QWidget* parent) : QWidget(parent), m_image(image) {
        m_pLabel = new QLabel;
        m_pLabel->setBackgroundRole(QPalette::Base);
        m_pLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
        m_pLabel->setScaledContents(true);

        m_pScrollArea = new QScrollArea(this);
        m_pScrollArea->setBackgroundRole(QPalette::Dark);
        m_pScrollArea->setWidget(m_pLabel);
        m_pScrollArea->setVisible(false);
        m_pScrollArea->setAlignment(Qt::AlignCenter);

        auto layout = new QVBoxLayout;
        layout->addWidget(m_pScrollArea);
        setLayout(std::move(layout));
        updateImage();
    }

    void ImageViewer::setImage(const Path& imagePath) {
        QImageReader reader(imagePath.as_cstr());
        reader.setAutoTransform(true);
        setImage(reader.read());
    }

    void ImageViewer::setImage(const QImage& image) {
        m_image = image;
        updateImage();
    }

    void ImageViewer::updateImage() const {
        if (m_image.isNull()) {
            m_pScrollArea->setVisible(false);
        } else {
            m_pLabel->setPixmap(QPixmap::fromImage(m_image));
            m_pLabel->adjustSize();
            m_pScrollArea->setVisible(true);
        }
    }
}
