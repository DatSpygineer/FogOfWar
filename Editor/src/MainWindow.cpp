#include "MainWindow.hpp"

#include <fow/Shared.hpp>

#include <QMenuBar>
#include <QDockWidget>
#include <QToolBar>

#include "ResourceHelper.hpp"
#include "assetEditor/AssetPackEditor.hpp"

namespace fow {
    extern Path EditorBaseDir;

    void MainWindow::setupFileMenu() {
        m_pFileMenu = menuBar()->addMenu("&File");
        m_pFileMenuNew = new QAction("New map..", this);
        m_pFileMenuNew->setIcon(LoadEditorIcon("Editor/icons/menu/new_small.png"));
        m_pFileMenu->addAction(m_pFileMenuNew);
        m_pFileMenuOpen = new QAction("Open map..", this);
        m_pFileMenuOpen->setIcon(LoadEditorIcon("Editor/icons/menu/open_small.png"));
        m_pFileMenu->addAction(m_pFileMenuOpen);
        m_pFileMenuSave = new QAction("Save map..", this);
        m_pFileMenuSave->setIcon(LoadEditorIcon("Editor/icons/menu/save_small.png"));
        m_pFileMenuSave->setEnabled(false);
        m_pFileMenu->addAction(m_pFileMenuSave);
        m_pFileMenuSaveAs = new QAction("Save map as..", this);
        m_pFileMenuSaveAs->setIcon(LoadEditorIcon("Editor/icons/menu/save_as_small.png"));
        m_pFileMenuSaveAs->setEnabled(false);
        m_pFileMenu->addAction(m_pFileMenuSaveAs);
        m_pFileMenu->addSeparator();
        m_pFileMenuEditorSettings = new QAction("Editor Settings", this);
        m_pFileMenuEditorSettings->setIcon(LoadEditorIcon("Editor/icons/menu/settings_small.png"));
        m_pFileMenu->addAction(m_pFileMenuEditorSettings);
        m_pFileExit = new QAction("Exit", this);
        m_pFileExit->setIcon(LoadEditorIcon("Editor/icons/menu/exit_small.png"));
        m_pFileMenu->addAction(m_pFileExit);

        connect(m_pFileExit, &QAction::triggered, this, &MainWindow::exitButton);
    }
    void MainWindow::setupEditMenu() {
        m_pMenuEdit  = menuBar()->addMenu("&Edit");
        m_pMenuEditUndo = m_pMenuEdit->addAction(LoadEditorIcon("Editor/icons/menu/undo_small.png"), "Undo");
        m_pMenuEditUndo->setEnabled(false);
        m_pMenuEditRedo = m_pMenuEdit->addAction(LoadEditorIcon("Editor/icons/menu/redo_small.png"), "Redo");
        m_pMenuEditRedo->setEnabled(false);
        m_pMenuEdit->addSeparator();
        m_pMenuEditCopy = m_pMenuEdit->addAction(LoadEditorIcon("Editor/icons/menu/copy_small.png"), "Copy");
        m_pMenuEditCopy->setEnabled(false);
        m_pMenuEditCut = m_pMenuEdit->addAction(LoadEditorIcon("Editor/icons/menu/cut_small.png"), "Cut");
        m_pMenuEditCut->setEnabled(false);
        m_pMenuEditPaste = m_pMenuEdit->addAction(LoadEditorIcon("Editor/icons/menu/paste_small.png"), "Paste");
        m_pMenuEditPaste->setEnabled(false);
        m_pMenuEdit->addSeparator();
        m_pMenuEditObjectProperties = m_pMenuEdit->addAction(LoadEditorIcon("Editor/icons/menu/properties_small.png"), "Object Properties");
        m_pMenuEditObjectProperties->setEnabled(false);
        m_pMenuEditMapProperties = m_pMenuEdit->addAction(LoadEditorIcon("Editor/icons/menu/map_properties_small.png"), "Map Properties");
    }
    void MainWindow::setupViewMenu() {
        m_pMenuView  = menuBar()->addMenu("&View");
        m_pMenuViewToggleWireframe = m_pMenuView->addAction(LoadEditorIcon("Editor/icons/menu/show_wireframe.png"), "Toggle wireframe");
        m_pMenuViewToggleWireframe->setCheckable(true);
        m_pMenuView->addSeparator();
        m_pMenuViewToggleGrid = m_pMenuView->addAction(LoadEditorIcon("Editor/icons/menu/show_grid_small.png"), "Toggle grid");
        m_pMenuViewToggleGrid->setCheckable(true);
        m_pMenuViewToggleBlockingTiles = m_pMenuView->addAction(LoadEditorIcon("Editor/icons/menu/show_blocking_small.png"), "Toggle passability display");
        m_pMenuViewToggleBlockingTiles->setCheckable(true);

        connect(m_pMenuViewToggleWireframe, &QAction::toggled, this, &MainWindow::toggleWireframe);
        connect(m_pMenuViewToggleGrid, &QAction::toggled, this, &MainWindow::toggleGrid);
    }
    void MainWindow::setupToolsMenu() {
        m_pMenuTools = menuBar()->addMenu("&Tools");

        m_pMenuToolsEditPacks = m_pMenuTools->addAction(LoadEditorIcon("Editor/icons/menu/gamedata_small.png"), "Asset pack editor");
        m_pMenuToolsValidateMap = m_pMenuTools->addAction(LoadEditorIcon("Editor/icons/menu/validate_small.png"), "Validate map");
        connect(m_pMenuToolsEditPacks, &QAction::triggered, this, &MainWindow::showAssetPackEditor);
    }
    void MainWindow::setupToolbar() {
        m_pToolBar = new QToolBar(this);
        m_pToolBarNew = m_pToolBar->addAction(LoadEditorIcon("Editor/icons/toolbar/new_large.png"), "New Map..");
        m_pToolBarOpen = m_pToolBar->addAction(LoadEditorIcon("Editor/icons/toolbar/open_large.png"), "Open Map..");
        m_pToolBarSave = m_pToolBar->addAction(LoadEditorIcon("Editor/icons/toolbar/save_large.png"), "Save Map");
        m_pToolBarSave->setEnabled(false);
        m_pToolBarSaveAs = m_pToolBar->addAction(LoadEditorIcon("Editor/icons/toolbar/save_as_large.png"), "Save Map as..");
        m_pToolBarSaveAs->setEnabled(false);
        m_pToolBar->addSeparator();
        m_pToolBarCopy = m_pToolBar->addAction(LoadEditorIcon("Editor/icons/toolbar/copy_large.png"), "Copy");
        m_pToolBarCopy->setEnabled(false);
        m_pToolBarCut = m_pToolBar->addAction(LoadEditorIcon("Editor/icons/toolbar/cut_large.png"), "Cut");
        m_pToolBarCut->setEnabled(false);
        m_pToolBarPaste = m_pToolBar->addAction(LoadEditorIcon("Editor/icons/toolbar/paste_large.png"), "Paste");
        m_pToolBarPaste->setEnabled(false);
        m_pToolBarProperties = m_pToolBar->addAction(LoadEditorIcon("Editor/icons/toolbar/properties_large.png"), "Object Properties");
        m_pToolBarProperties->setEnabled(false);
        m_pToolBar->addSeparator();
        m_pToolBarToggleWireframe = m_pToolBar->addAction(LoadEditorIcon("Editor/icons/toolbar/show_wireframe.png"), "Toggle wireframe");
        m_pToolBarToggleWireframe->setCheckable(true);
        m_pToolBarToggleGrid = m_pToolBar->addAction(LoadEditorIcon("Editor/icons/toolbar/show_grid.png"), "Toggle grid");
        m_pToolBarToggleGrid->setCheckable(true);
        m_pToolBarToggleBlockedTiles = m_pToolBar->addAction(LoadEditorIcon("Editor/icons/toolbar/show_blocked_tiles.png"), "Toggle passability display");
        m_pToolBarToggleBlockedTiles->setCheckable(true);
        m_pToolBarValidate = m_pToolBar->addAction(LoadEditorIcon("Editor/icons/toolbar/validate.png"), "Validate map");
        m_pToolBar->addSeparator();
        m_pToolBarUndo = m_pToolBar->addAction(LoadEditorIcon("Editor/icons/toolbar/undo.png"), "Undo");
        m_pToolBarUndo->setEnabled(false);
        m_pToolBarRedo = m_pToolBar->addAction(LoadEditorIcon("Editor/icons/toolbar/redo.png"), "Redo");
        m_pToolBarRedo->setEnabled(false);
        addToolBar(m_pToolBar);

        connect(m_pToolBarToggleWireframe, &QAction::toggled, this, &MainWindow::toggleWireframe);
        connect(m_pToolBarToggleGrid, &QAction::toggled, this, &MainWindow::toggleGrid);
    }

    MainWindow::MainWindow() {
        setupFileMenu();
        setupEditMenu();
        setupViewMenu();
        setupToolsMenu();

        m_pViewport = new EditorViewport(this);
        setCentralWidget(m_pViewport);

        setupToolbar();

        m_pToolsDockWidget = new QDockWidget(this);
        m_pToolsDockWidget->setFeatures(QDockWidget::NoDockWidgetFeatures);
        m_pToolsDockWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);

        m_pToolsTabWidget = new QTabWidget();

        m_pToolBoxObjects = new QToolBox();
        m_pToolBoxObjects->addItem(new QWidget(), "Category 1");
        m_pToolBoxObjects->addItem(new QWidget(), "Category 2");
        m_pToolBoxObjects->addItem(new QWidget(), "Category 3");
        m_pToolsTabWidget->addTab(m_pToolBoxObjects, "Objects");
        m_pToolsTabWidget->addTab(new QWidget(), "Terrain");
        m_pToolsTabWidget->addTab(new QWidget(), "Mask");
        m_pToolsTabWidget->addTab(new QWidget(), "Triggers");
        m_pToolsDockWidget->setWidget(m_pToolsTabWidget);

        addDockWidget(Qt::RightDockWidgetArea, m_pToolsDockWidget);
        setWindowTitle("Fog of War Editor");
        resize(1280, 720);
    }

    void MainWindow::exitButton() {
        close();
    }

    void MainWindow::toggleWireframe(const bool checked) const {
        m_pViewport->setDrawWireframe(checked);
        m_pViewport->update();
    }
    void MainWindow::toggleGrid(const bool checked) const {
        m_pViewport->setDrawGrid(checked);
        m_pViewport->update();
    }
    void MainWindow::showAssetPackEditor() {
        m_pViewport->setEnabled(false);
        auto editor = AssetPackEditor(this);
        editor.exec();
        m_pViewport->setEnabled(true);
        m_pViewport->updateView();
    }
}
