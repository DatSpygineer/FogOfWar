#include "AssetFileTree.hpp"

#include "util/ResourceHelper.hpp"

namespace fow {
    ArchiveFileTree::ArchiveFileTree(AssetInspector* inspector, const Path& projectRoot, const String& filter, const String& className, QWidget* parent)
        : QTreeWidget(parent), m_pInspector(inspector), m_sProjectRoot(projectRoot), m_sFilter(filter), m_sClassName(className) {
        setSortingEnabled(true);
        setColumnCount(1);
        setHeaderLabel("Name");
        connect(this, &QTreeWidget::currentItemChanged, this, &ArchiveFileTree::inspectSelection);

        if (filter.is_empty() && !className.is_empty()) {
            if (className == "Image") {
                m_sFilter = "*.png;*.bmp;*.jpg;*.jpeg;*.tga;*.dds";
            } else if (className == "Mesh") {
                m_sFilter = "*.glb;*.gltf;*.3ds;*.obj;*.dae;*.fbx;*.blend";
            } else {
                m_sFilter = "*.xml";
            }
        }
    }

    ArchiveFileTree::ArchiveFileTree(AssetInspector* inspector, const Path& projectRoot, const String& filter, QWidget* parent)
        : ArchiveFileTree(inspector, projectRoot, filter, "", parent) { }

    ArchiveFileTree::ArchiveFileTree(AssetInspector* inspector, const Path& projectRoot, QWidget* parent)
        : ArchiveFileTree(inspector, projectRoot, "", "", parent) { }

    void ArchiveFileTree::buildTree(const Path& root) {
        clear();
        for (const auto& entry : root.list_dir()) {
            int folderCounter = 0;
            buildTreeInternal(folderCounter, nullptr, entry);
        }
        sortByColumn(0, Qt::AscendingOrder);
        updateProjectDir(root);
    }

    void ArchiveFileTree::updateProjectDir(const Path& projectRoot) {
        m_sProjectRoot = projectRoot;
    }

    void ArchiveFileTree::buildTreeInternal(int& folderCounter, ArchiveFileTreeItem* parent, const Path& entry_path) {
        auto item = new ArchiveFileTreeItem(entry_path);
        const auto is_dir = entry_path.is_directory();
        item->setText(0, entry_path.filename().as_cstr());
        item->setData(0, Qt::UserRole, is_dir);

        if (is_dir) {
            item->setIcon(0, LoadEditorIcon("/icons/assetEdit/folder.png"));
        } else {
            const auto patterns = m_sFilter.split(';');
            auto filter_passed = false;
            for (const auto& pattern : patterns) {
                if (entry_path.matches(pattern)) {
                    filter_passed = true;
                    break;
                }
            }

            if (!m_sFilter.is_empty() && !filter_passed) {
                return;
            }
            if (entry_path.extension().equals(".xml", StringCompareType::CaseInsensitive) && !m_sClassName.is_empty()) {
                pugi::xml_document doc;
                doc.load_file(entry_path.as_cstr());
                if (!doc.child(m_sClassName.as_cstr())) {
                    return;
                }
            }

            auto icon_set = false;
            if (entry_path.extension().equals(".xml", StringCompareType::CaseInsensitive)) {
                pugi::xml_document doc;
                if (doc.load_file(entry_path.as_cstr()).status == pugi::status_ok) {
                    if (doc.child("Texture")) {
                        item->setIcon(0, LoadEditorIcon("/icons/assetEdit/texture.png"));
                    } else if (doc.child("Material")) {
                        item->setIcon(0, LoadEditorIcon("/icons/assetEdit/material.png"));
                    } else if (doc.child("Model")) {
                        item->setIcon(0, LoadEditorIcon("/icons/assetEdit/scene.png"));
                    } else if (doc.child("AnimSet")) {
                        item->setIcon(0, LoadEditorIcon("/icons/assetEdit/anim.png"));
                    } else if (doc.child("SoundInfo")) {
                        item->setIcon(0, LoadEditorIcon("/icons/assetEdit/sound_info.png"));
                    } else if (doc.child("Ambience")) {
                        item->setIcon(0, LoadEditorIcon("/icons/assetEdit/soundscape.png"));
                    } else if (doc.child("Pack")) {
                        item->setIcon(0, LoadEditorIcon("/icons/assetEdit/package.png"));
                    } else {
                        item->setIcon(0, LoadEditorIcon("/icons/assetEdit/xml.png"));
                    }
                    icon_set = true;
                }
            }
            if (entry_path.extension().equals(".json", StringCompareType::CaseInsensitive)) {
                if (entry_path.filename() == "Lang.json") {
                    item->setIcon(0, LoadEditorIcon("/icons/assetEdit/language_list.png"));
                } else if (entry_path.parent().filename() == "Lang") {
                    item->setIcon(0, LoadEditorIcon("/icons/assetEdit/language.png"));
                } else {
                    item->setIcon(0, LoadEditorIcon("/icons/assetEdit/json.png"));
                }
                icon_set = true;
            }
            if (entry_path.extension().equals(".lua", StringCompareType::CaseInsensitive)) {
                item->setIcon(0, LoadEditorIcon("/icons/assetEdit/script.png"));
                icon_set = true;
            }
            if (entry_path.extension().equals(".txt", StringCompareType::CaseInsensitive)) {
                item->setIcon(0, LoadEditorIcon("/icons/assetEdit/file_text.png"));
                icon_set = true;
            }
            if (entry_path.extension().equals_any({ ".cfg", ".ini", ".conf" }, StringCompareType::CaseInsensitive)) {
                item->setIcon(0, LoadEditorIcon("/icons/assetEdit/config.png"));
                icon_set = true;
            }
            if (entry_path.extension().equals(".csv", StringCompareType::CaseInsensitive)) {
                item->setIcon(0, LoadEditorIcon("/icons/assetEdit/table.png"));
                icon_set = true;
            }
            if (entry_path.extension().equals_any({ ".png", ".bmp", ".tga", ".dds", ".jpg", ".jpeg" }, StringCompareType::CaseInsensitive)) {
                item->setIcon(0, LoadEditorIcon("/icons/assetEdit/image.png"));
                icon_set = true;
            }
            if (entry_path.extension().equals_any({ ".glb", ".gltf", ".obj", ".fbx", ".dae" }, StringCompareType::CaseInsensitive)) {
                item->setIcon(0, LoadEditorIcon("/icons/assetEdit/mesh.png"));
                icon_set = true;
            }
            if (entry_path.extension().equals_any({ ".wav", ".mp3", ".ogg", ".flac" }, StringCompareType::CaseInsensitive)) {
                item->setIcon(0, LoadEditorIcon("/icons/assetEdit/sound.png"));
                icon_set = true;
            }

            if (!icon_set) {
                item->setIcon(0, LoadEditorIcon("/icons/assetEdit/file.png"));
            }
        }

        if (parent == nullptr) {
            if (is_dir) {
                insertTopLevelItem(folderCounter++, item);
            } else {
                addTopLevelItem(item);
            }
        } else {
            if (is_dir) {
                parent->insertChild(folderCounter++, item);
            } else {
                parent->addChild(item);
            }
        }
        if (is_dir) {
            int localFolderCounter = 0;
            for (const auto& entry : entry_path.list_dir()) {
                buildTreeInternal(localFolderCounter, item, entry);
            }
        }
    }

    void ArchiveFileTree::inspectSelection(const QTreeWidgetItem* current, const QTreeWidgetItem* previous) {
        DISCARD(previous);

        m_sSelectedPath = "";
        if (current == nullptr) {
            return;
        }
        auto parent = current->parent();
        while (parent != nullptr) {
            m_sSelectedPath = m_sSelectedPath.is_empty()
                ? Path(parent->text(0).toStdString())
                : Path(parent->text(0).toStdString()) / m_sSelectedPath;
            parent = parent->parent();
        }
        m_sSelectedPath = m_sProjectRoot / m_sSelectedPath / current->text(0).toStdString();
        if (m_pInspector != nullptr) {
            m_pInspector->inspectFile(m_sSelectedPath);
        }
        Debug::LogDebug(std::format("Selected file \"{}\"", m_sSelectedPath));
    }

    ArchiveFileSelectDialog::ArchiveFileSelectDialog(const String& title, const Path& projectRoot, const String& filter, const String& className, QWidget* parent)
        : QDialog(parent), m_sAssetRootPath(projectRoot) {
        setWindowTitle(title.as_cstr());
        resize(600, 600);
        auto layout = new QVBoxLayout;
        m_pArchiveFileTree = new ArchiveFileTree(nullptr, projectRoot, filter, className);
        m_pArchiveFileTree->buildTree(projectRoot);
        layout->addWidget(m_pArchiveFileTree, 7);

        auto buttons_layout = new QHBoxLayout;
        buttons_layout->addStretch();
        m_pSelectButton = new QPushButton(LoadEditorIcon("/icons/buttons/accept.png"), "Select");
        buttons_layout->addWidget(m_pSelectButton);
        m_pCancelButton = new QPushButton(LoadEditorIcon("/icons/buttons/cross.png"), "Cancel");
        buttons_layout->addWidget(m_pCancelButton);
        layout->addLayout(std::move(buttons_layout), 1);
        setLayout(std::move(layout));

        connect(m_pSelectButton, &QPushButton::clicked, this, &ArchiveFileSelectDialog::accept);
        connect(m_pCancelButton, &QPushButton::clicked, this, &ArchiveFileSelectDialog::reject);
        connect(m_pArchiveFileTree, &QTreeWidget::itemDoubleClicked, this, &ArchiveFileSelectDialog::accept);
    }
    ArchiveFileSelectDialog::ArchiveFileSelectDialog(const String& title, const Path& projectRoot, const String& filter, QWidget* parent) : ArchiveFileSelectDialog(title, projectRoot, filter, "", parent) { }
    ArchiveFileSelectDialog::ArchiveFileSelectDialog(const String& title, const Path& projectRoot, QWidget* parent) : ArchiveFileSelectDialog(title, projectRoot, "", parent) { }

    Path ArchiveFileSelectDialog::selectedPathRelative() const {
        return m_sSelectionPath.as_relative(m_sAssetRootPath);
    }

    void ArchiveFileSelectDialog::accept() {
        m_sSelectionPath = m_pArchiveFileTree->selectedPath();
        QDialog::accept();
    }
    void ArchiveFileSelectDialog::reject() {
        m_sSelectionPath = "";
        QDialog::reject();
    }
}
