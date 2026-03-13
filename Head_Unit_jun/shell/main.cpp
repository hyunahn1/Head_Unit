/**
 * @file main.cpp
 * @brief Head Unit Shell 진입점
 */

#include "ShellWindow.h"
#include <QApplication>
#include <QByteArray>

int main(int argc, char *argv[])
{
    // 플랫폼 자동 감지:
    //   - DISPLAY 또는 WAYLAND_DISPLAY가 설정되어 있으면 데스크탑 환경 → xcb/wayland 사용
    //   - 없으면 RPi4 임베디드 환경 → eglfs 사용
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
