/**
 * @file ShellWindow.cpp
 */

#include "ShellWindow.h"
#include "ModuleController.h"
#include "ModuleBridge.h"
#include "widgets/TabBar.h"
#include "widgets/StatusBar.h"
#include "widgets/GlowOverlay.h"
#include "widgets/SplashScreen.h"
#include "widgets/FallbackContentWidget.h"
#include "widgets/ReverseCameraWindow.h"
#include "MockVehicleDataProvider.h"
#include "VSomeIPClient.h"
#include "MockLedController.h"
#include "GearStateManager.h"
#include "IVehicleDataProvider.h"

#include <QVBoxLayout>
#include <QWidget>
#include <QWindow>
#include <QStackedWidget>
#include <QApplication>
#include <QScreen>
#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDateTime>

// 모듈 이름, 실행파일명, 소켓 경로
static const struct {
    const char *name;
    const char *exec;
    const char *socket;
} kModules[6] = {
    { "media",      "hu_module_media",      "/tmp/hu_shell_media.sock"      },
    { "youtube",    "hu_module_youtube",    "/tmp/hu_shell_youtube.sock"    },
    { "call",       "hu_module_call",       "/tmp/hu_shell_call.sock"       },
    { "navigation", "hu_module_navigation", "/tmp/hu_shell_navigation.sock" },
    { "ambient",    "hu_module_ambient",    "/tmp/hu_shell_ambient.sock"    },
    { "settings",   "hu_module_settings",  "/tmp/hu_shell_settings.sock"   },
};

ShellWindow::ShellWindow(QWidget *parent)
    : QMainWindow(parent)
{
    // 모듈 창을 QStackedWidget에 X11 embed해서 하나의 창처럼 동작
    setWindowFlags(Qt::FramelessWindowHint | Qt::Window);
    setGeometry(0, 0, WIN_W, WIN_H);
    setWindowTitle("PiRacer Head Unit");
    setStyleSheet("QMainWindow { background-color: #0D0D0F; }");

    m_vehicleData      = new MockVehicleDataProvider(this);
    m_vsomeipClient    = new VSomeIPClient(this);
    m_gearStateManager = new GearStateManager(this);
    m_ledController    = new MockLedController(this);

    setupUI();
    setupModules();
    setupConnections();
}

ShellWindow::~ShellWindow()
{
    for (int i = 0; i < MODULE_COUNT; ++i) {
        if (m_bridges[i])     m_bridges[i]->sendShutdown();
        if (m_controllers[i]) m_controllers[i]->terminate();
    }
}

void ShellWindow::setupUI()
{
    QWidget *central = new QWidget(this);
    setCentralWidget(central);

    auto *layout = new QVBoxLayout(central);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    m_tabBar = new TabBar(this);
    m_tabBar->setFixedHeight(TAB_H);
    layout->addWidget(m_tabBar);

    // 모듈 창을 X11 embed로 내장하는 스택 위젯
    m_stack = new QStackedWidget(this);
    m_stack->setFixedHeight(CONTENT_H);
    m_stack->setStyleSheet("background:#0D0D0F;");
    layout->addWidget(m_stack, 1);

    // 각 모듈 슬롯에 폴백 UI (GearPanel + 메시지) — embed 전까지 표시, 모듈 실패 시에도 기어 제어 가능
    static const char *tabNames[] = { "Media", "YouTube", "Call", "Navigation", "Lighting", "Settings" };
    for (int i = 0; i < MODULE_COUNT; ++i) {
        auto *ph = new FallbackContentWidget(
            m_gearStateManager,
            QString("Loading %1...").arg(tabNames[i]),
            m_stack
        );
        m_stack->addWidget(ph);
    }

    m_statusBar = new StatusBar(m_vehicleData, m_gearStateManager, this);
    m_statusBar->setFixedHeight(STATUS_H);
    layout->addWidget(m_statusBar);

    m_ambientGlow = new GlowOverlay(central);
    m_ambientGlow->setGeometry(0, 0, WIN_W, WIN_H);
    m_ambientGlow->raise();

    // 부팅 스플래시 — 로고 페이드인 후 콘텐츠로 전환
    auto *splash = new SplashScreen(central);
    splash->setGeometry(0, 0, WIN_W, WIN_H);
    splash->raise();
    connect(splash, &SplashScreen::finished, splash, &QWidget::deleteLater);
    connect(splash, &SplashScreen::finished, this, [this] { m_ambientGlow->raise(); });
}

void ShellWindow::setupModules()
{
    for (int i = 0; i < MODULE_COUNT; ++i) {
        m_bridges[i] = new ModuleBridge(kModules[i].socket, this);
        m_bridges[i]->listen();

        m_controllers[i] = new ModuleController(
            kModules[i].name,
            kModules[i].exec,
            kModules[i].socket,
            this
        );

        connect(m_controllers[i], &ModuleController::moduleStarted,
                this, &ShellWindow::onModuleStarted);
        connect(m_controllers[i], &ModuleController::moduleExited,
                this, &ShellWindow::onModuleExited);

        // 모듈이 준비되면 X11 winId로 스택에 embed
        connect(m_bridges[i], &ModuleBridge::moduleReady,
                this, [this, i](quint64 winId) {
            onModuleEmbedReady(i, winId);
        });

        // 기어 변경 요청
        connect(m_bridges[i], &ModuleBridge::gearChangeRequested,
                this, &ShellWindow::onGearChangeRequested);

        // Ambient 모듈만 색상 시그널 연결
        if (i == 4) { // ambient index
            connect(m_bridges[i], &ModuleBridge::ambientColorChanged,
                    this, &ShellWindow::onAmbientColorChanged);
            connect(m_bridges[i], &ModuleBridge::ambientOff,
                    this, &ShellWindow::onAmbientOff);
        }

        // Settings 모듈
        if (i == 5) {
            connect(m_bridges[i], &ModuleBridge::settingsChanged,
                    this, &ShellWindow::onSettingsChanged);
        }

        m_controllers[i]->launch();
    }

    // 시작 시 모두 숨기고 첫 탭(media)만 표시
    // 약간 딜레이 후 활성화 (모듈 초기화 시간 대기)
    QTimer::singleShot(500, this, [this] { switchToModule(0); });
}

void ShellWindow::setupConnections()
{
    connect(m_tabBar, &TabBar::tabSelected, this, &ShellWindow::onTabChanged);

    connect(m_gearStateManager, &GearStateManager::gearChanged,
            this, &ShellWindow::onGearChanged);

    // 차량 데이터 → 모든 모듈 브로드캐스트
    connect(m_vehicleData, &IVehicleDataProvider::speedChanged,
            this, &ShellWindow::broadcastVehicleSpeed);
    connect(m_vehicleData, &IVehicleDataProvider::batteryChanged,
            this, &ShellWindow::broadcastBattery);

    // VSOMEIP 상태 폴링 (1초마다) → 변경 시 모듈에 브로드캐스트
    m_ipcPollTimer = new QTimer(this);
    m_ipcPollTimer->setInterval(1000);
    connect(m_ipcPollTimer, &QTimer::timeout, this, [this] {
        const bool connected = m_vsomeipClient && m_vsomeipClient->isConnected();
        if (connected != m_lastIpcStatus) {
            m_lastIpcStatus = connected;
            broadcastIpcStatus(connected);
        }
    });
    m_ipcPollTimer->start();

    // 게임패드 기어 폴링 (500ms마다 파일 읽기)
    m_gearFilePollTimer = new QTimer(this);
    m_gearFilePollTimer->setInterval(500);
    connect(m_gearFilePollTimer, &QTimer::timeout, this, &ShellWindow::pollGamepadGear);
    m_gearFilePollTimer->start();
}

// ── 탭 전환 ──────────────────────────────────────────────────────────

void ShellWindow::onTabChanged(int index)
{
    if (index == m_activeIndex) return;
    switchToModule(index);
}

void ShellWindow::switchToModule(int index)
{
    if (index < 0 || index >= MODULE_COUNT) return;
    m_activeIndex = index;
    m_tabBar->setCurrentIndex(index);
    m_stack->setCurrentIndex(index);  // X11 embed: 스택만 전환하면 됨
}

// ── 기어 변경 ─────────────────────────────────────────────────────────

void ShellWindow::onGearChanged(GearState gear, const QString &source)
{
    Q_UNUSED(source)
    if (m_vsomeipClient) {
        m_vsomeipClient->publishGear(gear);
    }

    const bool isReverse = (static_cast<quint8>(gear) == 1); // GearState::R = 1
    if (isReverse) {
        if (!m_reverseCamera) {
            m_reverseCamera = new ReverseCameraWindow(this);
            m_reverseCamera->setAttribute(Qt::WA_DeleteOnClose);
            connect(m_reverseCamera, &QWidget::destroyed, this, [this] {
                m_reverseCamera = nullptr;
            });
            QScreen *screen = QApplication::primaryScreen();
            if (screen) {
                QRect geo = screen->availableGeometry();
                m_reverseCamera->move(
                    geo.x() + (geo.width()  - m_reverseCamera->width())  / 2,
                    geo.y() + (geo.height() - m_reverseCamera->height()) / 2 - 80
                );
            }
        }
        m_reverseCamera->show();
        m_reverseCamera->raise();
    } else {
        if (m_reverseCamera) m_reverseCamera->close();
    }

    broadcastGearState(gear);
}

void ShellWindow::onGearChangeRequested(GearState gear, const QString &source)
{
    m_gearStateManager->setGear(gear, source);
}

// ── 브로드캐스트 ──────────────────────────────────────────────────────

void ShellWindow::broadcastGearState(GearState gear)
{
    for (auto *b : m_bridges)
        if (b) b->sendGearState(gear);
}

void ShellWindow::broadcastVehicleSpeed(float kmh)
{
    for (auto *b : m_bridges)
        if (b) b->sendVehicleSpeed(kmh);
}

void ShellWindow::broadcastBattery(float v, float pct)
{
    for (auto *b : m_bridges)
        if (b) b->sendBattery(v, pct);
}

void ShellWindow::broadcastIpcStatus(bool connected)
{
    for (auto *b : m_bridges)
        if (b) b->sendIpcStatus(connected);
}

// ── 게임패드 기어 폴링 ───────────────────────────────────────────────

static QString gearToDirection(GearState g)
{
    switch (static_cast<quint8>(g)) {
    case 3: return "F";   // GearState::D
    case 1: return "R";   // GearState::R
    default: return "N";  // GearState::P (0) and GearState::N (2)
    }
}

void ShellWindow::pollGamepadGear()
{
    static const QString kPath = "/tmp/piracer_drive_mode.json";

    // 파일 존재 및 신선도 확인 (3초 이상 된 파일 무시)
    QFileInfo fi(kPath);
    if (!fi.exists()) return;
    if (fi.lastModified().msecsTo(QDateTime::currentDateTime()) > 3000) return;

    QFile f(kPath);
    if (!f.open(QIODevice::ReadOnly)) return;
    const QJsonDocument doc = QJsonDocument::fromJson(f.readAll());
    f.close();
    if (!doc.isObject()) return;

    const QString dir = doc.object().value("direction").toString().trimmed().toUpper();
    if (dir.isEmpty()) return;

    // 파일 방향이 바뀌지 않으면 처리 안 함 (echo 방지)
    if (dir == m_lastFileGearDir) return;
    m_lastFileGearDir = dir;

    // 현재 기어가 이미 같은 방향이면 무시 (P/N → 둘 다 "N"이므로 덮어쓰지 않음)
    const GearState currentGear = m_gearStateManager->gear();
    if (dir == gearToDirection(currentGear)) return;

    GearState newGear;
    if (dir == "F")      newGear = GearState::D;
    else if (dir == "R") newGear = GearState::R;
    else                 newGear = GearState::N;

    qDebug() << "[Shell] gamepad gear from file:" << dir;
    m_gearStateManager->setGear(newGear, "gamepad");
}

// ── 모듈 이벤트 수신 ─────────────────────────────────────────────────

void ShellWindow::onAmbientColorChanged(quint8 r, quint8 g, quint8 b, quint8 brightness)
{
    m_ambientGlow->setGlow(r, g, b, brightness);
}

void ShellWindow::onAmbientOff()
{
    m_ambientGlow->clearGlow();
}

void ShellWindow::onSettingsChanged(const QVariantMap &changes)
{
    Q_UNUSED(changes)
    // TODO: 속도 단위 변경 등 쉘 레벨 설정 반영
}

void ShellWindow::onModuleEmbedReady(int idx, quint64 winId)
{
    QWindow *foreign = QWindow::fromWinId(static_cast<WId>(winId));
    if (!foreign) {
        qWarning() << "[Shell] fromWinId failed for module" << idx;
        return;
    }

    // Make the embedded window visible
    foreign->show();

    QWidget *container = QWidget::createWindowContainer(foreign, m_stack);
    container->setMinimumSize(WIN_W, CONTENT_H);
    container->setFocusPolicy(Qt::StrongFocus);

    // placeholder를 container로 교체 (같은 인덱스 유지)
    QWidget *old = m_stack->widget(idx);
    m_stack->removeWidget(old);
    old->deleteLater();
    m_stack->insertWidget(idx, container);

    // 현재 탭이면 바로 표시
    if (m_activeIndex == idx)
        m_stack->setCurrentIndex(idx);

    qDebug() << "[Shell] module" << idx << "embedded, winId=" << winId;
}

void ShellWindow::onModuleStarted(const QString &name)
{
    qDebug() << "[Shell] module started:" << name;
}

void ShellWindow::onModuleExited(const QString &name, int exitCode)
{
    qWarning() << "[Shell] module exited:" << name << "code:" << exitCode;
}
