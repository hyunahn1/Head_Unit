/**
 * @file MainWindow.h
 * @brief Head Unit Main Window - TabBar + Content + StatusBar
 * @author Ahn Hyunjun
 * @date 2026-02-20
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>

class TabBar;
class StatusBar;
class MediaScreen;
class AmbientScreen;
class GearScreen;
class SettingsScreen;
class IVehicleDataProvider;
class GearStateManager;
class ILedController;

/**
 * @class MainWindow
 * @brief 4-tab layout: Media | Lighting | Gear | Settings
 * Architecture: docs/ARCHITECTURE_DESIGN.md
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onTabChanged(int index);

private:
    void setupUI();
    void setupConnections();
    void applyStyles();

    TabBar *m_tabBar;
    QStackedWidget *m_contentStack;
    StatusBar *m_statusBar;

    MediaScreen *m_mediaScreen;
    AmbientScreen *m_ambientScreen;
    GearScreen *m_gearScreen;
    SettingsScreen *m_settingsScreen;

    IVehicleDataProvider *m_vehicleData;   // Mock or VSomeIPClient
    GearStateManager *m_gearStateManager;
    ILedController *m_ledController;

    static constexpr int WINDOW_WIDTH = 1280;
    static constexpr int WINDOW_HEIGHT = 720;
    static constexpr int TAB_BAR_HEIGHT = 48;
    static constexpr int STATUS_BAR_HEIGHT = 36;
};

#endif // MAINWINDOW_H
