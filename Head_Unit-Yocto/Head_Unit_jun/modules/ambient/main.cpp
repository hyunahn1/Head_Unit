/**
 * @file main.cpp (ambient module)
 */
#include "AmbientService.h"
#include "AmbientWindow.h"
#include "ShellClient.h"
#include "GearStateManager.h"
#include "MockLedController.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("HU Ambient");

    const bool standalone = (argc < 2);
    const QString socketPath = standalone ? QString() : QString(argv[1]);

    auto *gearManager = new GearStateManager(&app);
    auto *led         = new MockLedController(&app);
    auto *service     = new AmbientService(led, &app);
    auto *window      = new AmbientScreen(led, gearManager);
    window->setWindowFlags(Qt::FramelessWindowHint | Qt::Window);

    if (standalone) {
        window->setGeometry(0, 40, 1024, 528);
        window->show();
    } else {
        auto *bridge = new ShellClient(socketPath, &app);

        // LED 색상 → shell 전달 (AmbientScreen이 직접 신호 발생)
        QObject::connect(window, &AmbientScreen::ambientColorChanged,
                         bridge, [bridge](uint8_t r, uint8_t g, uint8_t b, int brightness) {
            bridge->sendAmbientColor(r, g, b, static_cast<quint8>(brightness));
        });
        QObject::connect(window, &AmbientScreen::ambientOff,
                         bridge, &ShellClient::sendAmbientOff);
        QObject::connect(bridge, &ShellClient::gearStateUpdated,
                         gearManager, [gearManager](GearState g) {
            gearManager->setGear(g, "shell");
        });
        QObject::connect(gearManager, &GearStateManager::gearChanged,
                         bridge, [bridge](GearState g, const QString &src) {
            if (src != "shell")
                bridge->requestGearChange(g, src);
        });
        QObject::connect(bridge, &ShellClient::connected, [bridge, window, gearManager]() {
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
