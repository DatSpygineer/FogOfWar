#ifndef FOW_EDITOR_MAINWINDOW_HPP
#define FOW_EDITOR_MAINWINDOW_HPP

#include <QMainWindow>
#include <QToolBox>

#include "EditorViewport.hpp"
#include "GLView.hpp"

namespace fow {
    class MainWindow : public QMainWindow {
        QMenu* m_pFileMenu;
        QAction* m_pFileMenuNew;
        QAction* m_pFileMenuOpen;
        QAction* m_pFileMenuSave;
        QAction* m_pFileMenuSaveAs;
        QAction* m_pFileMenuEditorSettings;
        QAction* m_pFileExit;

        QMenu* m_pMenuEdit;
        QAction* m_pMenuEditUndo;
        QAction* m_pMenuEditRedo;
        QAction* m_pMenuEditCopy;
        QAction* m_pMenuEditCut;
        QAction* m_pMenuEditPaste;
        QAction* m_pMenuEditObjectProperties;
        QAction* m_pMenuEditMapProperties;

        QMenu* m_pMenuView;
        QAction* m_pMenuViewToggleWireframe;
        QAction* m_pMenuViewToggleGrid;
        QAction* m_pMenuViewToggleBlockingTiles;

        QMenu* m_pMenuTools;
        QAction* m_pMenuToolsEditPacks;
        QAction* m_pMenuToolsValidateMap;

        QToolBar* m_pToolBar;
        QAction* m_pToolBarNew;
        QAction* m_pToolBarOpen;
        QAction* m_pToolBarSave;
        QAction* m_pToolBarSaveAs;
        QAction* m_pToolBarCopy;
        QAction* m_pToolBarCut;
        QAction* m_pToolBarPaste;
        QAction* m_pToolBarProperties;
        QAction* m_pToolBarToggleWireframe;
        QAction* m_pToolBarToggleGrid;
        QAction* m_pToolBarToggleBlockedTiles;
        QAction* m_pToolBarValidate;
        QAction* m_pToolBarUndo;
        QAction* m_pToolBarRedo;

        EditorViewport* m_pViewport;
        QDockWidget* m_pToolsDockWidget;
        QTabWidget* m_pToolsTabWidget;
        QToolBox* m_pToolBoxObjects;
    public:
        MainWindow();

    private:
        void setupFileMenu();
        void setupEditMenu();
        void setupViewMenu();
        void setupToolsMenu();
        void setupToolbar();

    private slots:
        void exitButton();
        void toggleWireframe(bool checked) const;
        void toggleGrid(bool checked) const;

        void showAssetPackEditor();
    };
}

#endif