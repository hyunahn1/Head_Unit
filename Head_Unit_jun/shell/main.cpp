/**
 * @file main.cpp
 * @brief Head Unit Shell 진입점
 */

#include "ShellWindow.h"
#include <QApplication>
#include <QByteArray>

int main(int argc, char *argv[])
{
    // Wayland은 창 절대 위치를 지원하지 않으므로 XWayland(xcb)로 실행
    qputenv("QT_QPA_PLATFORM", "xcb");

    QApplication app(argc, argv);
    app.setApplicationName("PiRacer Head Unit Shell");
    app.setApplicationVersion("2.0.0");

    ShellWindow w;
    w.show();

    return app.exec();
}
