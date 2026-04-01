#ifndef FOW_EDITOR_ASSETPACKEDITOR_HPP
#define FOW_EDITOR_ASSETPACKEDITOR_HPP

#include <QHBoxLayout>
#include <QToolBar>
#include <QTableWidget>
#include <QFileDialog>
#include <QListWidget>
#include <QKeyEvent>

#include "util/ResourceHelper.hpp"
#include "widgets/RecentFilesButton.hpp"
#include "../widgets/AssetEditor/AssetInspector.hpp"

extern bool DevMode;

namespace fow {
    class RecentFilesButton;
    class ArchiveFileTree;

    class AssetPackEditor : public QDialog {
        QToolButton* m_pNewProjectButton;
        QToolButton* m_pOpenProjectButton;
        RecentFilesButton* m_pOpenRecentButton;
        QToolButton* m_pSaveArchiveButton;
        ArchiveFileTree* m_pAssetTree;
        AssetInspector* m_pInspector;

        Path m_sProjectBaseDir;
        Path m_sCurrentProjectDir;
    public:
        explicit AssetPackEditor(QWidget* parent);

        void openProject(const Path& path);
        [[nodiscard]] const Path& getProjectBaseDir() const { return m_sCurrentProjectDir; }
    private:
        void newPackage();
        void openPackage();
        void exportPackage();
    };
}

#endif