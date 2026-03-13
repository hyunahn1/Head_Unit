/**
 * @file main.cpp
 * @brief PiRacer Head Unit Application Entry Point
 * @author Ahn Hyunjun
 * @date 2026-02-20
 */

#include <QApplication>
#include "MainWindow.h"

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
#include <QtGlobal>
#endif

int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#endif

    QApplication app(argc, argv);

    app.setApplicationName("PiRacer Head Unit");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("PiRacer");

    MainWindow window;
    window.show();

    return app.exec();
}
