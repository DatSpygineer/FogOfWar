#include "AssetPackEditor.hpp"
#include "GameSettings.hpp"

#include <QTextEdit>
#include <QPushButton>
#include <QCheckBox>

extern bool DevMode;

namespace fow {
    class ArchiveFileTree : public QTreeWidget {
    public:
        ArchiveFileTree(QWidget* parent = nullptr) : QTreeWidget(parent) { }

        void buildTree(const Path& root) {
            for (const auto& entry : root.list_dir()) {
                buildTreeInternal(nullptr, entry);
            }
        }
    private:
        void buildTreeInternal(QTreeWidgetItem* parent, const Path& entry_path) {
            auto item = new QTreeWidgetItem;
            if (entry_path.is_directory()) {
                item->setIcon(0, LoadEditorIcon("Editor/icons/assetEdit/folder.png"));
            } else {
                auto icon_set = false;
                if (entry_path.extension().equals(".xml", StringCompareType::CaseInsensitive)) {
                    pugi::xml_document doc;
                    if (doc.load_file(entry_path.as_cstr()).status == pugi::status_ok) {
                        const String name = doc.root().name();
                        if (name == "Texture") {
                            item->setIcon(0, LoadEditorIcon("Editor/icons/assetEdit/texture.png"));
                        } else if (name == "Shader") {
                            item->setIcon(0, LoadEditorIcon("Editor/icons/assetEdit/shader.png"));
                        } else if (name == "Material") {
                            item->setIcon(0, LoadEditorIcon("Editor/icons/assetEdit/material.png"));
                        } else if (name == "Model") {
                            item->setIcon(0, LoadEditorIcon("Editor/icons/assetEdit/scene.png"));
                        } else if (name == "SoundInfo") {
                            item->setIcon(0, LoadEditorIcon("Editor/icons/assetEdit/sound_info.png"));
                        } else if (name == "Ambience") {
                            item->setIcon(0, LoadEditorIcon("Editor/icons/assetEdit/soundscape.png"));
                        } else if (name == "Package") {
                            item->setIcon(0, LoadEditorIcon("Editor/icons/assetEdit/package.png"));
                        } else {
                            item->setIcon(0, LoadEditorIcon("Editor/icons/assetEdit/xml.png"));
                        }
                        icon_set = true;
                    }
                }
                if (entry_path.extension().equals(".json", StringCompareType::CaseInsensitive)) {
                    if (entry_path.filename() == "Lang.json") {
                        item->setIcon(0, LoadEditorIcon("Editor/icons/assetEdit/language_list.png"));
                    } else if (entry_path.parent().filename() == "Lang") {
                        item->setIcon(0, LoadEditorIcon("Editor/icons/assetEdit/language.png"));
                    } else {
                        item->setIcon(0, LoadEditorIcon("Editor/icons/assetEdit/json.png"));
                    }
                    icon_set = true;
                }
                if (entry_path.extension().equals(".lua", StringCompareType::CaseInsensitive)) {
                    item->setIcon(0, LoadEditorIcon("Editor/icons/assetEdit/script.png"));
                    icon_set = true;
                }
                if (entry_path.extension().equals(".txt", StringCompareType::CaseInsensitive)) {
                    item->setIcon(0, LoadEditorIcon("Editor/icons/assetEdit/file_text.png"));
                    icon_set = true;
                }
                if (entry_path.extension().equals_any({ ".cfg", ".ini", ".conf" }, StringCompareType::CaseInsensitive)) {
                    item->setIcon(0, LoadEditorIcon("Editor/icons/assetEdit/config.png"));
                    icon_set = true;
                }
                if (entry_path.extension().equals_any({ ".png", ".bmp", ".tga", ".dds", ".jpg", ".jpeg" }, StringCompareType::CaseInsensitive)) {
                    item->setIcon(0, LoadEditorIcon("Editor/icons/assetEdit/image.png"));
                    icon_set = true;
                }
                if (entry_path.extension().equals_any({ ".glb", ".gltf", ".obj", ".fbx", ".dae" }, StringCompareType::CaseInsensitive)) {
                    item->setIcon(0, LoadEditorIcon("Editor/icons/assetEdit/mesh.png"));
                    icon_set = true;
                }
                if (entry_path.extension().equals_any({ ".wav", ".mp3", ".ogg", ".flac" }, StringCompareType::CaseInsensitive)) {
                    item->setIcon(0, LoadEditorIcon("Editor/icons/assetEdit/sound.png"));
                    icon_set = true;
                }

                if (!icon_set) {
                    item->setIcon(0, LoadEditorIcon("Editor/icons/assetEdit/file.png"));
                }
            }

            if (parent == nullptr) {
                addTopLevelItem(item);
            } else {
                parent->addChild(item);
            }
            if (entry_path.is_directory()) {
                for (const auto& entry : entry_path.list_dir()) {
                    buildTreeInternal(item, entry);
                }
            }
        };
    };

    class NewAssetPackDialog : public QDialog {
        QLineEdit* m_pNameEntry;
        QLineEdit* m_pAuthorsEntry;
        QTextEdit* m_pDescription;
        QCheckBox* m_pCheckBoxIsMod;
        QPushButton* m_pAcceptButton;
        QPushButton* m_pRejectButton;
    public:
        explicit NewAssetPackDialog(QWidget* parent = nullptr) : QDialog(parent) {
            auto layout = new QVBoxLayout;
            layout->addWidget(std::move(new QLabel("Name")));
            m_pNameEntry = new QLineEdit;
            layout->addWidget(m_pNameEntry);
            layout->addWidget(std::move(new QLabel("Authors")));
            m_pAuthorsEntry = new QLineEdit;
            layout->addWidget(m_pAuthorsEntry);
            layout->addWidget(std::move(new QLabel("Description")));
            m_pDescription = new QTextEdit;
            layout->addWidget(m_pDescription);

            if (DevMode) {
                m_pCheckBoxIsMod = new QCheckBox("Is Mod");
                m_pCheckBoxIsMod->setChecked(true);
                layout->addWidget(m_pCheckBoxIsMod);
            } else {
                m_pCheckBoxIsMod = nullptr;
            }

            layout->addStretch();
            auto buttons_layout = new QHBoxLayout;
            m_pAcceptButton = new QPushButton("Create");
            m_pRejectButton = new QPushButton("Cancel");
            connect(m_pAcceptButton, &QPushButton::clicked, this, &NewAssetPackDialog::accept);
            connect(m_pRejectButton, &QPushButton::clicked, this, &NewAssetPackDialog::reject);

            buttons_layout->addWidget(m_pAcceptButton);
            buttons_layout->addWidget(m_pRejectButton);
            buttons_layout->addStretch();

            layout->addLayout(std::move(buttons_layout));
            setLayout(std::move(layout));
        }

        void accept() override {
            if (m_pNameEntry->text().isEmpty()) {
                QMessageBox::warning(this, "No name specified!", "No name was given! Make sure the 'name' entry is not empty!");
                return;
            }
            QDialog::accept();
        }

        [[nodiscard]] FOW_CONSTEXPR String name() const { return m_pNameEntry->text().toStdString(); }
        [[nodiscard]] FOW_CONSTEXPR String authors() const { return m_pAuthorsEntry->text().toStdString(); }
        [[nodiscard]] FOW_CONSTEXPR String description() const { return m_pDescription->document()->toPlainText().toStdString(); }
        [[nodiscard]] FOW_CONSTEXPR bool is_mod() const { return m_pCheckBoxIsMod && m_pCheckBoxIsMod->isChecked(); }
    };

    AssetPackEditor::AssetPackEditor(QWidget* parent): QDialog(parent) {
        setWindowTitle("Asset pack editor");
        resize(1280, 720);

        auto layout = new QVBoxLayout;
        auto toolbar = new QToolBar;
        m_pNewProjectButton = new QToolButton;
        m_pNewProjectButton->setIcon(LoadEditorIcon("Editor/icons/toolbar/new_large.png"));
        m_pNewProjectButton->setToolTip("New project");
        toolbar->addWidget(m_pNewProjectButton);
        m_pOpenProjectButton = new QToolButton;
        m_pOpenProjectButton->setIcon(LoadEditorIcon("Editor/icons/toolbar/open_large.png"));
        m_pOpenProjectButton->setToolTip("Open project..");
        toolbar->addWidget(m_pOpenProjectButton);
        m_pImportArchiveButton = new QToolButton;
        m_pImportArchiveButton->setIcon(LoadEditorIcon("Editor/icons/toolbar/import.png"));
        m_pImportArchiveButton->setToolTip("Import package..");
        toolbar->addWidget(m_pImportArchiveButton);
        m_pExportArchiveButton = new QToolButton;
        m_pExportArchiveButton->setIcon(LoadEditorIcon("Editor/icons/toolbar/export.png"));
        m_pExportArchiveButton->setEnabled(false);
        m_pExportArchiveButton->setToolTip("Export project as package..");
        toolbar->addWidget(m_pExportArchiveButton);
        layout->addWidget(std::move(toolbar));
        layout->addSpacing(16);

        auto editor_layout = new QHBoxLayout;
        m_pAssetTree = new QTreeWidget;
        editor_layout->addWidget(m_pAssetTree, 3);
        auto preview_layout = new QVBoxLayout;
        m_pPreview = new GLView(nullptr);
        preview_layout->addWidget(m_pPreview, 1);
        m_pParameters = new QTableWidget;
        preview_layout->addWidget(m_pParameters, 2);
        m_pParameters->setColumnCount(2);
        m_pParameters->setHorizontalHeaderLabels(QStringList { "Name", "Value" });
        editor_layout->addLayout(std::move(preview_layout), 1);

        layout->addLayout(std::move(editor_layout));
        setLayout(std::move(layout));

        m_sProjectBaseDir = GetGameSettings().GamePath / "Editor" / "Projects";
        if (!m_sProjectBaseDir.exists()) {
            Path::CreateDirectory(m_sProjectBaseDir, true);
        }

        connect(m_pNewProjectButton,    &QToolButton::clicked, this, &AssetPackEditor::newPackage);
        connect(m_pOpenProjectButton,   &QToolButton::clicked, this, &AssetPackEditor::openPackage);
        connect(m_pImportArchiveButton, &QToolButton::clicked, this, &AssetPackEditor::importPackage);
        connect(m_pExportArchiveButton, &QToolButton::clicked, this, &AssetPackEditor::exportPackage);
    }

    void AssetPackEditor::newPackage() {
        auto dialog = NewAssetPackDialog(this);
        if (dialog.exec()) {

        }
    }
    void AssetPackEditor::openPackage() {
        auto fd = QFileDialog(this, "Select project to load", m_sProjectBaseDir.as_cstr());
        fd.setFileMode(QFileDialog::Directory);
        if (fd.exec()) {
            // TODO: Open project
        }
    }
    void AssetPackEditor::importPackage() {
        auto path = Path(QFileDialog::getOpenFileName(this, "Select pacakge to load", (GetGameSettings().GamePath / "mods").as_cstr(), "FogOfWar Asset package (*.zip *.pak)").toStdString());
        if (path.is_empty()) {
            return;
        }

        if ((m_sProjectBaseDir / path.filename_without_extension()).exists()) {
            if (QMessageBox::question(this, "Project already created", std::format("Project \"{}\" already exists, would you like to overwrite it?", path.filename_without_extension()).c_str()) != QMessageBox::Yes) {
                if (QMessageBox::question(this, "Open existing project", "Would you like to open the existing project?") != QMessageBox::Yes) {
                    return;
                }
            } else {
                // Unzip archive
            }
        }
        // Load project
    }
    void AssetPackEditor::exportPackage() {

    }
}
