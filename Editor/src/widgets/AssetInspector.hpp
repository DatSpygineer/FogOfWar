#ifndef FOW_EDITOR_ASSET_INSPECTOR_HPP
#define FOW_EDITOR_ASSET_INSPECTOR_HPP

#include <QStackedWidget>
#include <QTextEdit>
#include <QToolButton>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QListWidget>

#include <fow/Shared.hpp>

#include "ImageViewer.hpp"

namespace fow {
    class PropertyEditor;

    enum class AssetInspectorPage {
        NoPreview,
        Error,
        Text,
        Image,
        Sound,
        Dictionary,
        GameData
    };

    class TextInspector : public QWidget {
        QToolButton* m_pSaveButton;
        QToolButton* m_pRevertButton;
        QTextEdit* m_pTextEdit;
        Path m_sSourcePath;
    public:
        TextInspector(const Path& source_path, const String& text, QWidget* parent = nullptr);
        explicit TextInspector(QWidget* parent = nullptr) : TextInspector("", "", parent) { }

        void inspectFile(const Path& path);
        [[nodiscard]] FOW_CONSTEXPR const Path& getSourcePath() const noexcept { return m_sSourcePath; }
        [[nodiscard]] String getText() const;
    private:
        void setSourcePath(const Path& path);
        void setText(const String& text) const;

        void setSaveButtonEnabled() const;
        void saveFile() const;
        void revert();
    };

    enum class DictionaryFormat {
        Json,
        Xml
    };

    class DictionaryInspector : public QWidget {
        QToolButton* m_pSaveButton;
        QToolButton* m_pRevertButton;
        QTableWidget* m_pDictionary;
        Path m_sSourcePath;
        DictionaryFormat m_eFormat;
        String m_sRootName;
    public:
        explicit DictionaryInspector(QWidget* parent = nullptr);

        void inspectFile(const Path& path);
    protected:
        void keyPressEvent(QKeyEvent* event) override;

    private:
        void saveFile() const;
        void revert();
        void setSaveButtonEnabled(int row, int column) const;
        void showContextMenu(const QPoint& pos);
        void removeCurrentRow();
    };

    class AssetInspector : public QWidget {
        QStackedWidget* m_pInspectorPages;
        QLabel* m_pErrorLabel;
        TextInspector* m_pTextEditorEntry;
        ImageViewer* m_pImageViewer;
        DictionaryInspector* m_pDictionaryEdit;
        PropertyEditor* m_pGameDataEdit;

        Vector<QWidget*> m_widgets;
    public:
        explicit AssetInspector(QWidget* parent);

        void inspectFile(const Path& file);
        void deactivate() const;
    };
}

#endif