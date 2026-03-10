#include "RecentFilesButton.hpp"

namespace fow {
    RecentFilesButton::RecentFilesButton(const String& text, const QIcon& icon, const size_t max_entry_count, QWidget* parent) : RecentFilesButton(max_entry_count, parent) {
        setIcon(icon);
        setText(text.as_cstr());
        setPopupMode(MenuButtonPopup);
    }

    void RecentFilesButton::addFilePath(const Path& path) {
        QAction* action;
        // Move action to the front
        if (containsFilePath(path)) {
            if (const auto idx = findFilePath(path); idx > 0) {
                action = m_actions.at(idx);
                m_actions.erase(m_actions.begin() + idx);
                m_actions.push_front(action);
            }
            return;
        }

        if (m_uMaxEntryCount > 0 && m_actions.size() > m_uMaxEntryCount) {
            action = m_actions.front();
            DISCARD(disconnect(action));
            removeAction(action);
            m_actions.pop_back();
        }
        action = addAction(path.as_cstr());
        connect(action, &QAction::triggered, [this, action] {
            Q_EMIT fileSelected(Path(action->text().toStdString()));
        });
        m_actions.push_front(action);
    }

    void RecentFilesButton::addFilePaths(const Vector<Path>& paths) {
        for (const auto& path : paths) {
            addFilePath(path);
        }
    }
    void RecentFilesButton::addFilePaths(const Vector<String>& paths) {
        for (const auto& path : paths) {
            addFilePath(path);
        }
    }

    void RecentFilesButton::removeFilePath(const Path& path) {
        const auto idx = findFilePath(path);
        const auto action = m_actions.at(idx);
        m_actions.erase(m_actions.begin() + idx);
        DISCARD(disconnect(action));
        removeAction(action);
    }

    void RecentFilesButton::clearFilePaths() {
        for (const auto action : m_actions) {
            DISCARD(disconnect(action));
            removeAction(action);
        }
        m_actions.clear();
    }

    void RecentFilesButton::setMaxEntryCount(const size_t max_entry_count) {
        const auto current_size = m_actions.size();
        if (max_entry_count > 0 && current_size > max_entry_count) {
            const auto diff = current_size - max_entry_count;
            for (size_t i = 0; i < diff; ++i) {
                m_actions.pop_front();
            }
        }
        m_uMaxEntryCount = max_entry_count;
    }

    bool RecentFilesButton::containsFilePath(const Path& path) const {
        return findFilePath(path) > -1;
    }

    Vector<String> RecentFilesButton::listFilePaths() const {
        Vector<String> paths;
        for (const auto& path : m_actions) {
            paths.emplace_back(path->text().toStdString());
        }
        return paths;
    }

    ssize_t RecentFilesButton::findFilePath(const Path& path) const {
        const auto it = std::ranges::find_if(m_actions, [&path](const QAction* action) {
            return action->text().toStdString() == path.as_std_str();
        });
        if (it == m_actions.end()) {
            return -1;
        }
        return std::distance(m_actions.begin(), it);
    }
}
