/**
 * @file main.cpp
 * @brief Head Unit Shell 진입점
 */

#include "ShellWindow.h"
#include <QApplication>
#include <QByteArray>

int main(int argc, char *argv[])
{
    if (qgetenv("QT_QPA_PLATFORM").isEmpty()) {
        const bool hasX11     = !qgetenv("DISPLAY").isEmpty();
        const bool hasWayland = !qgetenv("WAYLAND_DISPLAY").isEmpty();
        if (hasWayland)
            qputenv("QT_QPA_PLATFORM", "wayland");
        else if (hasX11)
            qputenv("QT_QPA_PLATFORM", "xcb");
        else
            qputenv("QT_QPA_PLATFORM", "eglfs");
    }

    QApplication app(argc, argv);
    app.setApplicationName("PiRacer Head Unit Shell");
    app.setApplicationVersion("2.0.0");

    ShellWindow w;
    w.show();

    return app.exec();
}
