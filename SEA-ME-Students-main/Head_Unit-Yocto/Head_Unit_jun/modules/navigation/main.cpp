/**
 * @file main.cpp (navigation module)
 */
#include "NavigationService.h"
#include "NavigationWindow.h"
#include "ShellClient.h"
#include "GearStateManager.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("HU Navigation");

    const bool standalone = (argc < 2);
    const QString socketPath = standalone ? QString() : QString(argv[1]);

    auto *gearManager = new GearStateManager(&app);
    auto *service     = new NavigationService(&app);
    auto *window      = new NavigationScreen(gearManager);
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
        QObject::connect(bridge, &ShellClient::connected, [bridge, window]() {
            window->setAttribute(Qt::WA_DontShowOnScreen, true);
            window->show();
            window->setAttribute(Qt::WA_DontShowOnScreen, false);
            window->hide();
            bridge->notifyReady(window->winId());
        });
        QObject::connect(bridge, &ShellClient::shellShutdown, &app, &QApplication::quit);
        bridge->connectToShell();
    }
    return app.exec();
}
