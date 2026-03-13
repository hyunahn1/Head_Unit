/**
 * @file main.cpp (call module)
 */
#include "CallService.h"
#include "CallWindow.h"
#include "ShellClient.h"
#include "GearStateManager.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("HU Call");

    const bool standalone = (argc < 2);
    const QString socketPath = standalone ? QString() : QString(argv[1]);

    auto *gearManager = new GearStateManager(&app);
    auto *service     = new CallService(&app);
    auto *window      = new CallScreen(gearManager);
    window->setWindowFlags(Qt::FramelessWindowHint | Qt::Window);

    if (standalone) {
        window->setGeometry(0, 40, 1024, 528);
        window->show();
    } else {
        auto *bridge = new ShellClient(socketPath, &app);
        QObject::connect(bridge, &ShellClient::gearStateUpdated,
                         gearManager, [gearManager](GearState g) {
            gearManager->setGear(g, "shell");
        });
        QObject::connect(gearManager, &GearStateManager::gearChanged,
                         bridge, [bridge](GearState g, const QString &src) {
            if (src != "shell")
                bridge->requestGearChange(g, src);
        });
        QObject::connect(bridge, &ShellClient::shellShutdown, &app, &QApplication::quit);
        QObject::connect(bridge, &ShellClient::connected, [bridge, window]() {
            window->show();  // X11 window must be mapped before embedding
            QTimer::singleShot(50, bridge, [bridge, window]() {
                bridge->notifyReady(window->winId());
            });
		});
        bridge->connectToShell();
    }
    return app.exec();
}
