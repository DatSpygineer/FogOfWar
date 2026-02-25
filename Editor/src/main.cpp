#include "MainWindow.hpp"

#include <fow/Shared.hpp>

#include <QApplication>
#include <QMessageBox>
#include <QSurfaceFormat>

#include "GameSettings.hpp"

fow::Path EditorBaseDir;
bool DevMode = false;

int main(int argc, char** argv) {
    EditorBaseDir = fow::Path(argv[0]).parent();

    QApplication app(argc, argv);
    fow::Debug::Initialize(EditorBaseDir / "logs");

    const auto args = fow::ParseArgs(argc, argv);
    const auto settings = fow::InitializeGameSettings(args);
    if (const auto result = fow::Assets::Initialize(settings.GamePath / "data", settings.GameDataPacks, settings.ModPath); !result.has_value()) {
        QMessageBox::critical(nullptr, "Fatal error!", std::format("Failed to initialize asset handler: {}", result.error().message).c_str());
        return 1;
    }
    if (args.contains("-dev")) {
        DevMode = true;
    }

    QSurfaceFormat format;
    format.setSamples(4);
    format.setDepthBufferSize(24);
    format.setMajorVersion(4);
    format.setMinorVersion(6);
    format.setProfile(QSurfaceFormat::CoreProfile);
    QSurfaceFormat::setDefaultFormat(format);

    fow::MainWindow w;
    w.showMaximized();
    return app.exec();
}
