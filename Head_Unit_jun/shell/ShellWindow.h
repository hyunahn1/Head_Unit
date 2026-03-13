/**
 * @file ShellWindow.h
 * @brief Head Unit Shell — 단일 프로세스, 탭 전환으로 모듈 위젯 show/hide
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

// 모듈 위젯
class MediaWindow;
class YouTubeScreen;
class CallScreen;
class NavigationScreen;
class AmbientScreen;
class SettingsScreen;

// 차량 데이터
class IVehicleDataProvider;
class VSomeIPClient;
class GearStateManager;
class ILedController;

enum class GearState : quint8;

class ShellWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ShellWindow(QWidget *parent = nullptr);
    ~ShellWindow();

private slots:
    void onTabChanged(int index);
    void onGearChanged(GearState gear, const QString &source);

    // Ambient 모듈 → GlowOverlay
    void onAmbientColorChanged(quint8 r, quint8 g, quint8 b, quint8 brightness);
    void onAmbientOff();

    // Settings 모듈
    void onSettingsChanged(const QVariantMap &changes);

    // 게임패드 기어 동기화 (파일 폴링)
    void pollGamepadGear();

private:
    void setupUI();
    void setupConnections();
    void switchToModule(int index);
    void broadcastGearState(GearState gear);
    void broadcastVehicleSpeed(float kmh);
    void broadcastBattery(float v, float pct);
    void broadcastIpcStatus(bool connected);

    // ── UI 컴포넌트 ───────────────────────────────────────────────────
    TabBar              *m_tabBar       = nullptr;
    StatusBar           *m_statusBar    = nullptr;
    GlowOverlay         *m_ambientGlow  = nullptr;
    ReverseCameraWindow *m_reverseCamera = nullptr;
    QStackedWidget      *m_stack        = nullptr;

    // ── 모듈 위젯 (in-process) ────────────────────────────────────────
    MediaWindow     *m_mediaWidget      = nullptr;
    YouTubeScreen   *m_youtubeWidget    = nullptr;
    CallScreen      *m_callWidget       = nullptr;
    NavigationScreen *m_navigationWidget = nullptr;
    AmbientScreen   *m_ambientWidget    = nullptr;
    SettingsScreen  *m_settingsWidget   = nullptr;

    // ── 차량 데이터 ───────────────────────────────────────────────────
    IVehicleDataProvider *m_vehicleData      = nullptr;
    VSomeIPClient        *m_vsomeipClient    = nullptr;
    GearStateManager     *m_gearStateManager = nullptr;
    ILedController       *m_ledController    = nullptr;

    int  m_activeIndex   = 0;
    bool m_lastIpcStatus = false;

    QTimer  *m_ipcPollTimer      = nullptr;
    QTimer  *m_gearFilePollTimer = nullptr;
    QString  m_lastFileGearDir;

    // 레이아웃 상수
    static constexpr int TAB_H    = 40;
    static constexpr int STATUS_H = 32;

    int m_winW = 1024;
    int m_winH = 600;
};

#endif // SHELLWINDOW_H
