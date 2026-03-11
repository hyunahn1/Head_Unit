/**
 * @file main.cpp
 * @brief Head Unit Shell 진입점
 */

#include "ShellWindow.h"
#include <QApplication>
#include <QByteArray>

int main(int argc, char *argv[])
{
    // Desktop: use xcb (X11) or wayland
    // Embedded RPi: use eglfs
    // Auto-detect if not set
    if (qgetenv("QT_QPA_PLATFORM").isEmpty()) {
        #ifdef __arm__
            qputenv("QT_QPA_PLATFORM", "eglfs");
        #else
            // Desktop Linux - let Qt auto-detect (xcb or wayland)
        #endif
    }

    QApplication app(argc, argv);
    app.setApplicationName("PiRacer Head Unit Shell");
    app.setApplicationVersion("2.0.0");

    ShellWindow w;
    w.show();

    return app.exec();
}
