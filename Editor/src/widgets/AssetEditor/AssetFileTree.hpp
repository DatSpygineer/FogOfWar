#ifndef FOW_EDITOR_ASSET_FILE_TREE_HPP
#define FOW_EDITOR_ASSET_FILE_TREE_HPP

#include <qdialog.h>
#include <QWidget>
#include <QTreeWidget>
#include <QPushButton>
#include <QMenu>
#include <fow/Shared.hpp>

#include "AssetInspector.hpp"

namespace fow {
    class ArchiveFileTreeItem : public QTreeWidgetItem {
        Path m_sPath;
    public:
        explicit ArchiveFileTreeItem(const Path& path) : QTreeWidgetItem(), m_sPath(path) {
            setText(0, m_sPath.filename_without_extension().as_cstr());
            setData(0, Qt::UserRole, path.is_directory());
        }

        bool operator<(const QTreeWidgetItem& other) const override {
            int column = treeWidget()->sortColumn();
            bool thisIsFolder = data(0, Qt::UserRole).toBool();
            bool otherIsFolder = other.data(0, Qt::UserRole).toBool();
            if (thisIsFolder != otherIsFolder) {
                return thisIsFolder;
            }
            return text(column).toLower() < other.text(column).toLower();
        }
    };

    class ArchiveFileTree : public QTreeWidget {
        AssetInspector* m_pInspector;
        Path m_sProjectRoot;
        String m_sFilter, m_sClassName;
        Path m_sSelectedPath = "";
    public:
        ArchiveFileTree(AssetInspector* inspector, const Path& projectRoot, const String& filter, const String& className, QWidget* parent = nullptr);
        ArchiveFileTree(AssetInspector* inspector, const Path& projectRoot, const String& filter, QWidget* parent = nullptr);
        explicit ArchiveFileTree(AssetInspector* inspector, const Path& projectRoot, QWidget* parent = nullptr);

        void buildTree(const Path& root);
        void updateProjectDir(const Path& projectRoot);

        [[nodiscard]] FOW_CONSTEXPR const Path& selectedPath() const { return m_sSelectedPath; }
    private:
        void buildTreeInternal(int& folderCounter, ArchiveFileTreeItem* parent, const Path& entry_path);
        void inspectSelection(const QTreeWidgetItem* current, const QTreeWidgetItem* previous);
    };

    class ArchiveFileSelectDialog : public QDialog {
        Path m_sSelectionPath;
        Path m_sAssetRootPath;
        ArchiveFileTree* m_pArchiveFileTree;
        QPushButton* m_pSelectButton;
        QPushButton* m_pCancelButton;
    public:
        ArchiveFileSelectDialog(const String& title, const Path& projectRoot, const String& filter, const String& className, QWidget* parent = nullptr);
        ArchiveFileSelectDialog(const String& title, const Path& projectRoot, const String& filter, QWidget* parent = nullptr);
        ArchiveFileSelectDialog(const String& title, const Path& projectRoot, QWidget* parent = nullptr);

        [[nodiscard]] FOW_CONSTEXPR const Path& selectedPath()         const { return m_sSelectionPath; }
        [[nodiscard]] Path selectedPathRelative() const;

        void accept() override;
        void reject() override;
    };
}

#endif