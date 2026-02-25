#ifndef FOW_EDITOR_ASSETPACKEDITOR_HPP
#define FOW_EDITOR_ASSETPACKEDITOR_HPP

#include <QLabel>
#include <QMessageBox>
#include <QHBoxLayout>
#include <QToolButton>
#include <QLineEdit>
#include <QToolBar>
#include <QTreeWidget>
#include <QTableWidget>
#include <QFileDialog>

#include "AssetPackEditor.hpp"
#include "GLView.hpp"
#include "ResourceHelper.hpp"

namespace fow {
    class AssetPackEditor : public QDialog {
        QToolButton* m_pNewProjectButton;
        QToolButton* m_pOpenProjectButton;
        QToolButton* m_pImportArchiveButton;
        QToolButton* m_pExportArchiveButton;
        QTreeWidget* m_pAssetTree;
        GLView* m_pPreview;
        QTableWidget* m_pParameters;
        Path m_sProjectBaseDir;
        Path m_sCurrentProjectDir;
    public:
        explicit AssetPackEditor(QWidget* parent);

    private:
        void newPackage();
        void openPackage();
        void importPackage();
        void exportPackage();
    };
}

#endif