#ifndef FOW_EDITOR_RECENT_FILES_BUTTON_HPP
#define FOW_EDITOR_RECENT_FILES_BUTTON_HPP

#include <fow/Shared.hpp>
#include <QToolButton>

namespace fow {
    class RecentFilesButton : public QToolButton {
        Q_OBJECT
        Deque<QAction*> m_actions;
        size_t m_uMaxEntryCount;
    public:
        explicit RecentFilesButton(const size_t maxEntryCount = 0, QWidget* parent = nullptr) : QToolButton(parent), m_uMaxEntryCount(maxEntryCount) { setPopupMode(MenuButtonPopup); }
        RecentFilesButton(const String& text, const QIcon& icon, size_t max_entry_count = 0, QWidget* parent = nullptr);
        explicit RecentFilesButton(const String& text, const size_t maxEntryCount = 0, QWidget* parent = nullptr) : RecentFilesButton(text, QIcon(), maxEntryCount, parent) { }
        explicit RecentFilesButton(const QIcon& icon, const size_t maxEntryCount = 0, QWidget* parent = nullptr) : RecentFilesButton("", icon, maxEntryCount, parent) { }

        void addFilePath(const Path& path);
        void addFilePaths(const Vector<Path>& paths);
        void addFilePaths(const Vector<String>& paths);
        void removeFilePath(const Path& path);
        void clearFilePaths();
        void setMaxEntryCount(size_t max_entry_count);
        [[nodiscard]] bool containsFilePath(const Path& path) const;
        [[nodiscard]] FOW_CONSTEXPR size_t getMaxEntryCount() const { return m_uMaxEntryCount; }

        [[nodiscard]] Vector<String> listFilePaths() const;
    Q_SIGNALS:
        void fileSelected(const Path& path);
    protected:
        ssize_t findFilePath(const Path& path) const;
    };
}

#endif
