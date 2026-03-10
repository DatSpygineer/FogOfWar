#include "AssetFileTree.hpp"

#include <QMessageBox>
#include <QVBoxLayout>
#include <QLabel>
#include <QKeyEvent>
#include <QHeaderView>

#include "util/ResourceHelper.hpp"
#include "PropertyEditor.hpp"

namespace fow {
    TextInspector::TextInspector(const Path& source_path, const String& text, QWidget* parent) : QWidget(parent), m_sSourcePath(source_path) {
        auto layout = new QVBoxLayout;
        auto button_layout = new QHBoxLayout;
        m_pSaveButton = new QToolButton;
        m_pSaveButton->setIcon(LoadEditorIcon("Editor/icons/menu/save_small.png"));
        m_pSaveButton->setToolTip("Save text");
        m_pSaveButton->setEnabled(false);
        m_pRevertButton = new QToolButton;
        m_pRevertButton->setIcon(LoadEditorIcon("Editor/icons/menu/refresh.png"));
        m_pRevertButton->setToolTip("Revert changes");
        m_pRevertButton->setEnabled(false);
        button_layout->addWidget(m_pSaveButton);
        button_layout->addWidget(m_pRevertButton);
        button_layout->addStretch();
        layout->addLayout(std::move(button_layout), 1);
        m_pTextEdit = new QTextEdit;
        m_pTextEdit->setText(text.as_cstr());
        layout->addWidget(m_pTextEdit, 9);
        setLayout(std::move(layout));

        connect(m_pTextEdit,     &QTextEdit::textChanged, this, &TextInspector::setSaveButtonEnabled);
        connect(m_pSaveButton,   &QToolButton::clicked,   this, &TextInspector::saveFile);
        connect(m_pRevertButton, &QToolButton::clicked,   this, &TextInspector::revert);
    }

    void TextInspector::inspectFile(const Path& path) {
        const auto result = Files::ReadAllText(path);
        if (!result.has_value()) {
            QMessageBox::critical(this, "Failed to load file", std::format("Failed to load text file \"{}\"", path).c_str());
            return;
        }
        setSourcePath(path);
        setText(result.value());
        m_pSaveButton->setEnabled(false);
    }

    void TextInspector::setSourcePath(const Path& path) {
        m_sSourcePath = path;
    }

    void TextInspector::setText(const String& text) const {
        m_pTextEdit->setText(text.as_cstr());
    }
    String TextInspector::getText() const {
        return m_pTextEdit->toPlainText().toStdString();
    }

    void TextInspector::setSaveButtonEnabled() const {
        if (!m_sSourcePath.is_empty()) {
            m_pSaveButton->setEnabled(true);
        }
    }

    void TextInspector::saveFile() const {
        if (const auto result = Files::WriteAllText(m_sSourcePath, m_pTextEdit->toPlainText().toStdString()); !result.has_value()) {
            QMessageBox::critical(nullptr, "Save failed", std::format("Failed to save file \"{}\": {}", m_sSourcePath, result.error().message).c_str());
        } else {
            m_pSaveButton->setEnabled(false);
        }
    }

    void TextInspector::revert() {
        if (QMessageBox::question(this, "Confirm revert", "Would you like to revert changes?") == QMessageBox::Yes) {
            inspectFile(m_sSourcePath);
        }
    }

    DictionaryInspector::DictionaryInspector(QWidget* parent) : QWidget(parent) {
        auto layout = new QVBoxLayout;
        auto buttons_layout = new QHBoxLayout;
        m_pSaveButton = new QToolButton;
        m_pSaveButton->setIcon(LoadEditorIcon("Editor/icons/menu/save_small.png"));
        m_pSaveButton->setToolTip("Save table");
        m_pSaveButton->setEnabled(false);
        m_pRevertButton = new QToolButton;
        m_pRevertButton->setIcon(LoadEditorIcon("Editor/icons/menu/refresh.png"));
        m_pRevertButton->setToolTip("Revert changes");
        m_pRevertButton->setEnabled(false);
        buttons_layout->addWidget(m_pSaveButton);
        buttons_layout->addWidget(m_pRevertButton);
        buttons_layout->addStretch();
        layout->addLayout(std::move(buttons_layout), 1);
        m_pDictionary = new QTableWidget;
        m_pDictionary->setColumnCount(2);
        m_pDictionary->setHorizontalHeaderLabels({ "Name", "Value" });
        m_pDictionary->setContextMenuPolicy(Qt::CustomContextMenu);
        m_pDictionary->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
        layout->addWidget(m_pDictionary, 9);
        setLayout(std::move(layout));

        connect(m_pSaveButton,   &QToolButton::clicked,                     this, &DictionaryInspector::saveFile);
        connect(m_pRevertButton, &QToolButton::clicked,                     this, &DictionaryInspector::revert);
        connect(m_pDictionary,   &QTableWidget::cellChanged,                this, &DictionaryInspector::setSaveButtonEnabled);
        connect(m_pDictionary,   &QTableWidget::customContextMenuRequested, this, &DictionaryInspector::showContextMenu);
    }

    void DictionaryInspector::inspectFile(const Path& path) {
        m_pDictionary->clear();
        m_pDictionary->setHorizontalHeaderLabels({ "Name", "Value" });
        m_sSourcePath = path;
        const auto ext = path.extension();
        if (ext.equals(".xml", StringCompareType::CaseInsensitive)) {
            m_eFormat = DictionaryFormat::Xml;
        } else if (ext.equals(".json", StringCompareType::CaseInsensitive)) {
            m_eFormat = DictionaryFormat::Json;
        }

        switch (m_eFormat) {
            case DictionaryFormat::Json: {
                std::ifstream ifs(path.as_std_path());
                nlohmann::json data = nlohmann::json::parse(ifs);
                m_pDictionary->setRowCount(data.size());
                size_t row = 0;
                for (auto it = data.begin(); it != data.end(); ++it) {
                    m_pDictionary->setItem(row, 0, new QTableWidgetItem(it.key().c_str()));
                    m_pDictionary->setItem(row, 1, new QTableWidgetItem(it.value().get<std::string>().c_str()));
                    ++row;
                }
                m_sRootName = "";
            } break;
            case DictionaryFormat::Xml: {
                pugi::xml_document doc;
                doc.load_file(path.as_cstr());
                size_t row = 0;
                for (const auto& item : doc.children()) {
                    m_pDictionary->insertRow(row);
                    m_pDictionary->setItem(row, 0, new QTableWidgetItem(item.name()));
                    m_pDictionary->setItem(row, 1, new QTableWidgetItem(item.child_value()));
                    ++row;
                }
                m_sRootName = doc.root().name();
            } break;
        }
        m_pRevertButton->setEnabled(false);
        m_pSaveButton->setEnabled(false);
    }

    void DictionaryInspector::keyPressEvent(QKeyEvent* event) {
        QWidget::keyPressEvent(event);
        if (event->key() == Qt::Key_Delete) {
            removeCurrentRow();
        }
    }

    void DictionaryInspector::saveFile() const {
        std::ofstream ofs(m_sSourcePath.as_std_path());
        switch (m_eFormat) {
            case DictionaryFormat::Json: {
                ofs << "{" << std::endl;
                const auto count = m_pDictionary->rowCount();
                for (size_t i = 0; i < count; ++i) {
                    ofs << "    \"" << m_pDictionary->item(i, 0)->text().toStdString() << "\": \"" << m_pDictionary->item(i, 1)->text().toStdString() << "\"";
                    if (i < count - 1) {
                        ofs << ',';
                    }
                    ofs << std::endl;
                }
                ofs << "}" << std::endl;
            } break;
            case DictionaryFormat::Xml: {
                ofs << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>" << std::endl;
                ofs << "<" << m_sRootName << ">" << std::endl;
                for (size_t i = 0; i < m_pDictionary->rowCount(); ++i) {
                    const auto key = m_pDictionary->item(i, 0)->text().toStdString();
                    const auto value = m_pDictionary->item(i, 1)->text().toStdString();
                    ofs << "    <" << key << ">" << value << "</" << key << ">" << std::endl;
                }
                ofs << "</" << m_sRootName << ">" << std::endl;
            } break;
        }
        m_pRevertButton->setEnabled(false);
        m_pSaveButton->setEnabled(false);
    }

    void DictionaryInspector::revert() {
        if (QMessageBox::question(this, "Confirm revert", "Would you like to revert changes?") == QMessageBox::Yes) {
            inspectFile(m_sSourcePath);
        }
    }

    void DictionaryInspector::setSaveButtonEnabled(int row, int column) const {
        m_pRevertButton->setEnabled(true);
        m_pSaveButton->setEnabled(true);
    }
    void DictionaryInspector::showContextMenu(const QPoint& pos) {
        auto menu = QMenu(this);

        auto currentRow = m_pDictionary->currentRow();
        if (currentRow < 0) {
            currentRow = m_pDictionary->rowCount();
        }
        auto act_insert_before = menu.addAction(LoadEditorIcon("Editor/icons/menu/table_rows_insert_above.png"), "Insert row above");
        auto act_insert_after  = menu.addAction(LoadEditorIcon("Editor/icons/menu/table_rows_insert_below.png"), "Insert row below");
        menu.addSeparator();
        auto act_remove        = menu.addAction(LoadEditorIcon("Editor/icons/menu/table_rows_delete.png"), "Remove row", QKeySequence::Delete);

        connect(act_insert_before, &QAction::triggered, this, [this, &currentRow] {
            m_pDictionary->insertRow(currentRow);
        });
        connect(act_insert_after, &QAction::triggered, this, [this, &currentRow] {
            m_pDictionary->insertRow(currentRow + 1);
        });
        connect(act_remove, &QAction::triggered, this, &DictionaryInspector::removeCurrentRow);

        menu.exec(m_pDictionary->mapToGlobal(pos));
    }

    void DictionaryInspector::removeCurrentRow() {
        auto items = m_pDictionary->selectedItems();
        if (items.isEmpty()) {
            return;
        }

        if (QMessageBox::question(this, "Confirm removal", "Would you like to remove the selected row(s)?") == QMessageBox::Yes) {
            for (const auto& item : items) {
                m_pDictionary->removeRow(item->row());
            }
        }

        m_pRevertButton->setEnabled(true);
        m_pSaveButton->setEnabled(true);
    }

    AssetInspector::AssetInspector(QWidget* parent) : QWidget(parent) {
        auto layout = new QVBoxLayout;
        m_pInspectorPages = new QStackedWidget;
        // No Preview
        auto label = new QLabel("No Preview");
        label->setAlignment(Qt::AlignCenter);
        m_pInspectorPages->addWidget(std::move(label));
        // Error
        m_pErrorLabel = new QLabel("Failed to parse file");
        m_pInspectorPages->addWidget(m_pErrorLabel);
        // Text
        m_pTextEditorEntry = new TextInspector;
        m_pInspectorPages->addWidget(m_pTextEditorEntry);
        // Image
        m_pImageViewer = new ImageViewer;
        m_pInspectorPages->addWidget(m_pImageViewer);
        auto temp_sound = new QLabel("Not implemented");
        temp_sound->setAlignment(Qt::AlignCenter);
        // Sound
        m_pInspectorPages->addWidget(std::move(temp_sound));
        // Dictionary
        m_pDictionaryEdit = new DictionaryInspector;
        m_pInspectorPages->addWidget(m_pDictionaryEdit);
        // Game data
        m_pGameDataEdit = new PropertyEditor(parent);
        m_pInspectorPages->addWidget(m_pGameDataEdit);

        layout->addWidget(m_pInspectorPages);
        setLayout(std::move(layout));
    }

    void AssetInspector::inspectFile(const Path& file) {
        if (file.is_directory()) {
            m_pInspectorPages->setCurrentIndex(static_cast<int>(AssetInspectorPage::NoPreview));
            return;
        }

        const auto ext = file.extension();
        if (ext.equals_any({ ".png", ".bmp", ".jpg", ".jpeg", ".tga", ".dds" }, StringCompareType::CaseInsensitive)) {
            m_pImageViewer->setImage(file);
            m_pInspectorPages->setCurrentIndex(static_cast<int>(AssetInspectorPage::Image));
        } else if (ext.equals_any({ ".txt", ".ini", ".cfg", ".conf", ".lua" }, StringCompareType::CaseInsensitive)) {
            m_pTextEditorEntry->inspectFile(file);
            m_pInspectorPages->setCurrentIndex(static_cast<int>(AssetInspectorPage::Text));
        } else if (ext.equals(".xml", StringCompareType::CaseInsensitive)) {
            const auto src = Files::ReadAllText(file);
            if (!src.has_value()) {
                QMessageBox::critical(this, "Error", std::format("Failed to parse xml file \"{}\": {}", file, src.error().message).c_str());
                m_pInspectorPages->setCurrentIndex(static_cast<int>(AssetInspectorPage::Error));
                return;
            }
            pugi::xml_document doc;
            if (const auto result = doc.load_string(src->as_cstr()); result.status != pugi::status_ok) {
                QMessageBox::critical(this, "Error", std::format("Failed to parse xml file \"{}\": {}", file, result.description()).c_str());
                m_pInspectorPages->setCurrentIndex(static_cast<int>(AssetInspectorPage::Error));
            }

            auto inspector_set = false;
            if (doc.child("Texture")) {
                m_pInspectorPages->setCurrentIndex(static_cast<int>(AssetInspectorPage::GameData));
                m_pGameDataEdit->inspectAsset(file, "Texture");
                inspector_set = true;
            } else if (doc.child("Material")) {
                m_pInspectorPages->setCurrentIndex(static_cast<int>(AssetInspectorPage::GameData));
                m_pGameDataEdit->inspectAsset(file, "Material");
                inspector_set = true;
            } else if (doc.child("Model")) {
                m_pInspectorPages->setCurrentIndex(static_cast<int>(AssetInspectorPage::GameData));
                m_pGameDataEdit->inspectAsset(file, "Model");
                inspector_set = true;
            } else if (doc.child("AnimSet")) {
                m_pInspectorPages->setCurrentIndex(static_cast<int>(AssetInspectorPage::GameData));
                m_pGameDataEdit->inspectAsset(file, "AnimSet");
                inspector_set = true;
            } else if (doc.child("SoundInfo")) {
                m_pInspectorPages->setCurrentIndex(static_cast<int>(AssetInspectorPage::GameData));
                m_pGameDataEdit->inspectAsset(file, "SoundInfo");
                inspector_set = true;
            } else if (doc.child("Ambience")) {
                m_pInspectorPages->setCurrentIndex(static_cast<int>(AssetInspectorPage::GameData));
                m_pGameDataEdit->inspectAsset(file, "Ambience");
                inspector_set = true;
            } else if (doc.child("Pack")) {
                m_pInspectorPages->setCurrentIndex(static_cast<int>(AssetInspectorPage::GameData));
                m_pGameDataEdit->inspectAsset(file, "Pack");
                inspector_set = true;
            }

            if (!inspector_set) { // NOLINT: Game data inspectors are not implemented yet!
                m_pTextEditorEntry->inspectFile(file);
                m_pInspectorPages->setCurrentIndex(static_cast<int>(AssetInspectorPage::Text));
            }
        } else  if (ext.equals(".json", StringCompareType::CaseInsensitive)) {
            if (file.filename() == "Lang.json" || file.parent().filename_without_extension() == "Lang") {
                m_pDictionaryEdit->inspectFile(file);
                m_pInspectorPages->setCurrentIndex(static_cast<int>(AssetInspectorPage::Dictionary));
            } else {
                m_pTextEditorEntry->inspectFile(file);
                m_pInspectorPages->setCurrentIndex(static_cast<int>(AssetInspectorPage::Text));
            }
        } else {
            m_pInspectorPages->setCurrentIndex(static_cast<int>(AssetInspectorPage::NoPreview));
        }
    }

    void AssetInspector::deactivate() const {
        m_pInspectorPages->setCurrentIndex(0);
    }
}
