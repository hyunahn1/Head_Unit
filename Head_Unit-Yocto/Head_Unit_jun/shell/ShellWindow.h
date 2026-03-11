/**
 * @file ShellWindow.h
 * @brief Head Unit Shell — TabBar + StatusBar + 모듈 프로세스 조율
 *
 * QStackedWidget 없음. 탭 전환은 ModuleBridge를 통해 모듈 창에
 * ShowModule / HideModule 메시지를 보내는 방식으로 처리.
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
class ModuleController;
class ModuleBridge;
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
    void onModuleStarted(const QString &name);
    void onModuleExited(const QString &name, int exitCode);
    void onModuleEmbedReady(int idx, quint64 winId);

    // 모듈 → 쉘 이벤트
    void onGearChangeRequested(GearState gear, const QString &source);
    void onAmbientColorChanged(quint8 r, quint8 g, quint8 b, quint8 brightness);
    void onAmbientOff();
    void onSettingsChanged(const QVariantMap &changes);

    // 게임패드 기어 동기화 (파일 폴링)
    void pollGamepadGear();

private:
    void setupUI();
    void setupModules();
    void setupConnections();
    void switchToModule(int index);
    void broadcastGearState(GearState gear);
    void broadcastVehicleSpeed(float kmh);
    void broadcastBattery(float v, float pct);
    void broadcastIpcStatus(bool connected);

    // ── UI 컴포넌트 ───────────────────────────────────────────────────
    TabBar               *m_tabBar       = nullptr;
    StatusBar            *m_statusBar    = nullptr;
    GlowOverlay          *m_ambientGlow  = nullptr;
    ReverseCameraWindow  *m_reverseCamera = nullptr;
    QStackedWidget       *m_stack        = nullptr;

    // ── 차량 데이터 ───────────────────────────────────────────────────
    IVehicleDataProvider *m_vehicleData       = nullptr;
    VSomeIPClient       *m_vsomeipClient     = nullptr;   // Gear publish → Instrument Cluster
    GearStateManager     *m_gearStateManager  = nullptr;
    ILedController       *m_ledController     = nullptr;

    // ── 모듈 프로세스 / 브릿지 ───────────────────────────────────────
    static constexpr int MODULE_COUNT = 6;
    ModuleController *m_controllers[MODULE_COUNT] {};
    ModuleBridge     *m_bridges[MODULE_COUNT]     {};

    int m_activeIndex = 0;
    bool m_lastIpcStatus = false;
    QTimer *m_ipcPollTimer = nullptr;
    QString m_lastFileGearDir;           // 마지막으로 읽은 파일 방향 ("F"|"R"|"N"|"")
    QTimer *m_gearFilePollTimer = nullptr;

    // 레이아웃 상수
    static constexpr int WIN_W      = 1024;
    static constexpr int WIN_H      = 600;
    static constexpr int TAB_H      = 40;
    static constexpr int STATUS_H   = 32;
    static constexpr int CONTENT_Y  = TAB_H;
    static constexpr int CONTENT_H  = WIN_H - TAB_H - STATUS_H;
};

#endif // SHELLWINDOW_H
