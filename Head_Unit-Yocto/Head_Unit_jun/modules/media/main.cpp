/**
 * @file main.cpp (media module)
 *
 * standalone 실행: ./hu_module_media
 * shell 관리 모드: ./hu_module_media /tmp/hu_shell_media.sock
 */

#include "MediaWindow.h"
#include "ShellClient.h"
#include "GearStateManager.h"

#include <QApplication>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("HU Media");

    const bool standalone = (argc < 2);
    const QString socketPath = standalone ? QString() : QString(argv[1]);

    auto *gearManager = new GearStateManager(&app);
    auto *window      = new MediaWindow(gearManager);
    window->setWindowFlags(Qt::FramelessWindowHint | Qt::Window);
    window->setAttribute(Qt::WA_TranslucentBackground, false);

    if (standalone) {
        window->setGeometry(0, 40, 1024, 528);
        window->show();
        qDebug() << "[hu_module_media] standalone mode";
    } else {
        auto *bridge = new ShellClient(socketPath, &app);
        QObject::connect(bridge, &ShellClient::gearStateUpdated,
                         gearManager, [gearManager](GearState g) {
            gearManager->setGear(g, "shell");
        });
        // GearPanel 입력 → shell → VSomeIPClient::publishGear → IC
        QObject::connect(gearManager, &GearStateManager::gearChanged,
                         bridge, [bridge](GearState g, const QString &src) {
            if (src != "shell")  // shell에서 온 것은 echo 방지
                bridge->requestGearChange(g, src);
        });
        QObject::connect(bridge, &ShellClient::shellShutdown, &app, &QApplication::quit);
        QObject::connect(bridge, &ShellClient::connected, [bridge, window]() {
            // Create native window for embedding (but don't show as top-level)
            window->setAttribute(Qt::WA_DontShowOnScreen, true);
            window->show();  // Creates native window
            window->setAttribute(Qt::WA_DontShowOnScreen, false);
            window->hide();  // Hide again - shell will show after embedding
            bridge->notifyReady(window->winId());
        });

        bridge->connectToShell();
    }

    return app.exec();
}
