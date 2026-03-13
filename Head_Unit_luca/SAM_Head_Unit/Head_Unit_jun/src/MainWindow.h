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
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include "ipc/IVehicleDataProvider.h"

class TabBar;
class StatusBar;
class ReverseCameraWindow;
class MediaScreen;
#ifndef HU_PRE_API
class YouTubeScreen;
#endif
class CallScreen;
class NavigationScreen;
class AmbientScreen;
class SettingsScreen;
class GlowOverlay;
class IVehicleDataProvider;
class GearStateManager;
class ILedController;
class VSomeIPClient;
class QTimer;

/**
 * @class MainWindow
 * @brief Tab layout: Media | Call | Navigation | Lighting | Settings
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
    void onGearChanged();

private:
    void setupUI();
    void setupConnections();
    void applyStyles();
    void writeDriveModeSnapshot(GearState gear);
#ifndef HU_PRE_API
    void ensureYouTubeScreen();
    void releaseYouTubeScreen();  // 탭 떠날 때 WebEngine 해제
#endif
#ifdef HU_WEBENGINE_AVAILABLE
    void ensureNavigationScreen();
    void releaseNavigationScreen();  // 탭 떠날 때 WebEngine 해제
#endif

    TabBar *m_tabBar;
    QStackedWidget *m_contentStack;
    StatusBar *m_statusBar;

    MediaScreen *m_mediaScreen;
#ifndef HU_PRE_API
    YouTubeScreen *m_youtubeScreen;
    QWidget *m_youtubePlaceholder;  // 탭 클릭 시 YouTubeScreen으로 교체 (WebEngine 지연 로드)
#endif
    CallScreen *m_callScreen;
    NavigationScreen *m_navigationScreen;
#ifdef HU_WEBENGINE_AVAILABLE
    QWidget *m_navigationPlaceholder;  // 탭 클릭 시 OSM 맵으로 교체 (WebEngine 지연 로드)
#endif
    AmbientScreen *m_ambientScreen;
    SettingsScreen *m_settingsScreen;

    IVehicleDataProvider *m_vehicleData;   // Mock or VSomeIPClient
    VSomeIPClient *m_vsomeipClient;        // Gear publish
    GearStateManager *m_gearStateManager;
    ILedController *m_ledController;
    GlowOverlay *m_ambientGlow;            // Full-window ambient glow
    QTimer *m_driveModeTimer;              // Periodic /tmp snapshot for cluster
    ReverseCameraWindow *m_reverseCamera;  // Shown when gear is R
    bool m_animating = false;              // Prevent overlapping slide animations

    int m_windowWidth;                     // 1024 고정
    int m_windowHeight;                    // 600 고정

    static constexpr int WINDOW_WIDTH_DEFAULT = 1024;
    static constexpr int WINDOW_HEIGHT_DEFAULT = 600;
    static constexpr int TAB_BAR_HEIGHT = 40;
    static constexpr int STATUS_BAR_HEIGHT = 32;
};

#endif // MAINWINDOW_H
