#include "AssetPackEditor.hpp"

#include <QSpinBox>
#include <QPushButton>
#include <QCheckBox>
#include <QLineEdit>
#include <QMessageBox>

#include "util/GameSettings.hpp"
#include "util/Zipper.hpp"
#include "widgets/AssetFileTree.hpp"

extern fow::Path EditorBaseDir;

namespace fow {
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
        m_pNewProjectButton->setToolTip("New package");
        toolbar->addWidget(m_pNewProjectButton);
        m_pOpenProjectButton = new QToolButton;
        m_pOpenProjectButton->setIcon(LoadEditorIcon("Editor/icons/toolbar/open_large.png"));
        m_pOpenProjectButton->setToolTip("Open package..");
        toolbar->addWidget(m_pOpenProjectButton);
        m_pOpenRecentButton = new RecentFilesButton(LoadEditorIcon("Editor/icons/toolbar/open_recent_large.png"));
        m_pOpenRecentButton->setToolTip("Open recent packages");

        const auto recent_list_path = EditorBaseDir / "recent_packages.txt";
        if (recent_list_path.exists()) {
            if (const auto result = Files::ReadAllLines(recent_list_path); result.has_value()) {
                m_pOpenRecentButton->addFilePaths(result.value());
            }
        }

        toolbar->addWidget(m_pOpenRecentButton);
        m_pSaveArchiveButton = new QToolButton;
        m_pSaveArchiveButton->setIcon(LoadEditorIcon("Editor/icons/toolbar/save_large.png"));
        m_pSaveArchiveButton->setEnabled(false);
        m_pSaveArchiveButton->setToolTip("Save package..");
        toolbar->addWidget(m_pSaveArchiveButton);
        layout->addWidget(std::move(toolbar));
        layout->addSpacing(16);

        // auto editor_layout = new QHBoxLayout;
        m_pInspector = new AssetInspector(this);
        m_pAssetTree = new ArchiveFileTree(m_pInspector, m_sCurrentProjectDir);
        // editor_layout->addWidget(m_pAssetTree, 3);
        // editor_layout->addWidget(m_pInspector, 1);
        layout->addWidget(m_pInspector, 2);
        layout->addWidget(m_pAssetTree, 1);

        // layout->addLayout(std::move(editor_layout));
        setLayout(std::move(layout));

        m_sProjectBaseDir = GetGameSettings().GamePath / "Editor" / "Projects";
        if (!m_sProjectBaseDir.exists()) {
            Path::CreateDirectory(m_sProjectBaseDir, true);
        }

        connect(m_pNewProjectButton,  &QToolButton::clicked, this, &AssetPackEditor::newPackage);
        connect(m_pOpenProjectButton, &QToolButton::clicked, this, &AssetPackEditor::openPackage);
        connect(m_pOpenRecentButton,  &RecentFilesButton::fileSelected, [this](const Path& path) {
            openProject(path);
        });
        connect(m_pSaveArchiveButton, &QToolButton::clicked, this, &AssetPackEditor::exportPackage);
    }

    void AssetPackEditor::openProject(const Path& path) {
        if (path.filename() != "Pack.xml" && !(path / "Pack.xml").exists()) {
            QMessageBox::critical(this, "Invalid project!", std::format("Failed to load project \"{}\": The project folder must contain 'Pack.xml'!", path).c_str());
            return;
        }

        m_sCurrentProjectDir = path.parent();
        m_pOpenRecentButton->addFilePath(path);
        m_pInspector->deactivate();

        m_pAssetTree->clearSelection();
        m_pAssetTree->buildTree(m_sCurrentProjectDir);
        Files::WriteAllLines(EditorBaseDir / "recent_packages.txt", m_pOpenRecentButton->listFilePaths());
    }

    void AssetPackEditor::newPackage() {
        auto dialog = NewAssetPackDialog(this);
        if (dialog.exec()) {

        }
    }
    void AssetPackEditor::openPackage() {
        auto path = Path(QFileDialog::getOpenFileName(this, "Select package to load", GetGameSettings().GamePath.as_cstr(), "FogOfWar Asset package (*.zip *.pak Pack.xml)").toStdString());
        if (path.is_empty()) {
            return;
        }

        Path projectDir;
        if (path.extension().equals_any({ ".zip", ".pak" }, StringCompareType::CaseInsensitive)) {
            const auto packageSourceDir = m_sProjectBaseDir / path.filename_without_extension();
            auto unzipSource = true;
            if (packageSourceDir.exists()) {
                if (QMessageBox::question(this, "Project already created", std::format("Project \"{}\" already exists, would you like to overwrite it?", path.filename_without_extension()).c_str()) != QMessageBox::Yes) {
                    if (QMessageBox::question(this, "Open existing project", "Would you like to open the existing project?") != QMessageBox::Yes) {
                        return;
                    }
                    unzipSource = false;
                } else {
                    DISCARD(packageSourceDir.remove_all());
                }
            }

            Path::CreateDirectory(packageSourceDir, true);

            if (unzipSource) {
                if (const auto result = Unzip(path, packageSourceDir); !result.has_value()) {
                    QMessageBox::critical(this, "Error", result.error().message.as_cstr());
                    return;
                }
            }
            projectDir = packageSourceDir / "Pack.xml";
        } else {
            projectDir = path.parent();
        }
        openProject(projectDir);
    }
    void AssetPackEditor::exportPackage() {

    }
}
