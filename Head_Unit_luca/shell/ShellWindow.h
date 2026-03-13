/**
 * @file ShellWindow.h
 * @brief Head Unit Shell — single-process, all modules loaded in-process
 */

#ifndef SHELLWINDOW_H
#define SHELLWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QTimer>

class TabBar;
class StatusBar;
class GlowOverlay;
class SplashScreen;
class ReverseCameraWindow;
class IVehicleDataProvider;
class VSomeIPClient;
class GearStateManager;
class ILedController;

// Module widgets
class MediaWindow;
class YouTubeScreen;
class CallScreen;
class NavigationScreen;
class AmbientScreen;
class SettingsScreen;

enum class GearState : quint8;

class ShellWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ShellWindow(QWidget *parent = nullptr);
    ~ShellWindow() override = default;

private slots:
    void onTabChanged(int index);
    void onGearChanged(GearState gear, const QString &source);
    void pollGamepadGear();

private:
    void setupUI();
    void setupConnections();
    void switchToModule(int index);

    // ── UI 컴포넌트 ───────────────────────────────────────────────────
    TabBar               *m_tabBar        = nullptr;
    StatusBar            *m_statusBar     = nullptr;
    GlowOverlay          *m_ambientGlow   = nullptr;
    ReverseCameraWindow  *m_reverseCamera = nullptr;
    QStackedWidget       *m_stack         = nullptr;

    // ── 인-프로세스 모듈 위젯 ─────────────────────────────────────────
    MediaWindow      *m_mediaWidget      = nullptr;
    YouTubeScreen    *m_youtubeWidget    = nullptr;
    CallScreen       *m_callWidget       = nullptr;
    NavigationScreen *m_navigationWidget = nullptr;
    AmbientScreen    *m_ambientWidget    = nullptr;
    SettingsScreen   *m_settingsWidget   = nullptr;

    // ── 차량 데이터 ───────────────────────────────────────────────────
    IVehicleDataProvider *m_vehicleData      = nullptr;
    VSomeIPClient        *m_vsomeipClient    = nullptr;
    GearStateManager     *m_gearStateManager = nullptr;
    ILedController       *m_ledController    = nullptr;

    int     m_activeIndex    = 0;
    bool    m_lastIpcStatus  = false;
    QTimer *m_ipcPollTimer   = nullptr;
    QString m_lastFileGearDir;
    QTimer *m_gearFilePollTimer = nullptr;

    // 동적 화면 크기
    int m_winW = 1024;
    int m_winH = 600;

    static constexpr int TAB_H    = 40;
    static constexpr int STATUS_H = 32;
};

#endif // SHELLWINDOW_H
