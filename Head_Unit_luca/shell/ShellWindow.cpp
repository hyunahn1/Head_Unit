/**
 * @file ShellWindow.cpp
 * @brief Single-process Head Unit Shell — all modules loaded in-process
 */

#include "ShellWindow.h"
#include "widgets/TabBar.h"
#include "widgets/StatusBar.h"
#include "widgets/GlowOverlay.h"
#include "widgets/SplashScreen.h"
#include "widgets/ReverseCameraWindow.h"
#include "MockVehicleDataProvider.h"
#include "VSomeIPClient.h"
#include "MockLedController.h"
#include "GearStateManager.h"
#include "IVehicleDataProvider.h"

// In-process module widgets
#include "../modules/media/ui/MediaWindow.h"
#include "../modules/youtube/ui/YouTubeWindow.h"
#include "../modules/call/ui/CallWindow.h"
#include "../modules/navigation/ui/NavigationWindow.h"
#include "../modules/ambient/ui/AmbientWindow.h"
#include "../modules/settings/ui/SettingsWindow.h"

#include <QVBoxLayout>
#include <QWidget>
#include <QApplication>
#include <QScreen>
#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDateTime>

ShellWindow::ShellWindow(QWidget *parent)
    : QMainWindow(parent)
{
    QScreen *screen = QApplication::primaryScreen();
    const QRect geo = screen ? screen->availableGeometry() : QRect(0, 0, 1024, 600);
    m_winW = geo.width();
    m_winH = geo.height();

    setWindowFlags(Qt::FramelessWindowHint | Qt::Window);
    setGeometry(geo);
    setWindowTitle("PiRacer Head Unit");
    setStyleSheet("QMainWindow { background-color: #0D0D0F; }");

    m_vehicleData      = new MockVehicleDataProvider(this);
    m_vsomeipClient    = new VSomeIPClient(this);
    m_gearStateManager = new GearStateManager(this);
    m_ledController    = new MockLedController(this);

    setupUI();
    setupConnections();
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

    m_stack = new QStackedWidget(this);
    m_stack->setStyleSheet("background:#0D0D0F;");
    layout->addWidget(m_stack, 1);

    // 모듈 위젯 인-프로세스 생성
    m_mediaWidget      = new MediaWindow(m_gearStateManager, m_stack);
    m_youtubeWidget    = new YouTubeScreen(m_gearStateManager, m_stack);
    m_callWidget       = new CallScreen(m_gearStateManager, m_stack);
    m_navigationWidget = new NavigationScreen(m_gearStateManager, m_stack);
    m_ambientWidget    = new AmbientScreen(m_ledController, m_gearStateManager, m_stack);
    m_settingsWidget   = new SettingsScreen(m_gearStateManager, m_stack);

    m_stack->addWidget(m_mediaWidget);
    m_stack->addWidget(m_youtubeWidget);
    m_stack->addWidget(m_callWidget);
    m_stack->addWidget(m_navigationWidget);
    m_stack->addWidget(m_ambientWidget);
    m_stack->addWidget(m_settingsWidget);
    m_stack->setCurrentIndex(0);

    m_statusBar = new StatusBar(m_vehicleData, m_gearStateManager, this);
    m_statusBar->setFixedHeight(STATUS_H);
    layout->addWidget(m_statusBar);

    m_ambientGlow = new GlowOverlay(central);
    m_ambientGlow->setGeometry(0, 0, m_winW, m_winH);
    m_ambientGlow->raise();

    auto *splash = new SplashScreen(central);
    splash->setGeometry(0, 0, m_winW, m_winH);
    splash->raise();
    connect(splash, &SplashScreen::finished, splash, &QWidget::deleteLater);
    connect(splash, &SplashScreen::finished, this, [this] { m_ambientGlow->raise(); });
}

void ShellWindow::setupConnections()
{
    connect(m_tabBar, &TabBar::tabSelected, this, &ShellWindow::onTabChanged);
    connect(m_gearStateManager, &GearStateManager::gearChanged,
            this, &ShellWindow::onGearChanged);

    // Ambient 모듈 → GlowOverlay
    connect(m_ambientWidget, &AmbientScreen::ambientColorChanged,
            this, [this](uint8_t r, uint8_t g, uint8_t b, int brightness) {
        m_ambientGlow->setGlow(r, g, b, brightness);
    });
    connect(m_ambientWidget, &AmbientScreen::ambientOff,
            m_ambientGlow, &GlowOverlay::clearGlow);

    // VSomeIP 상태 폴링 (1초마다)
    m_ipcPollTimer = new QTimer(this);
    m_ipcPollTimer->setInterval(1000);
    connect(m_ipcPollTimer, &QTimer::timeout, this, [this] {
        const bool connected = m_vsomeipClient && m_vsomeipClient->isConnected();
        if (connected != m_lastIpcStatus) {
            m_lastIpcStatus = connected;
            if (m_settingsWidget)
                m_settingsWidget->updateIpcStatus(connected);
        }
    });
    m_ipcPollTimer->start();

    // 게임패드 기어 폴링 (500ms마다)
    m_gearFilePollTimer = new QTimer(this);
    m_gearFilePollTimer->setInterval(500);
    connect(m_gearFilePollTimer, &QTimer::timeout, this, &ShellWindow::pollGamepadGear);
    m_gearFilePollTimer->start();
}

void ShellWindow::onTabChanged(int index)
{
    if (index == m_activeIndex) return;
    switchToModule(index);
}

void ShellWindow::switchToModule(int index)
{
    if (index < 0 || index >= m_stack->count()) return;
    m_activeIndex = index;
    m_tabBar->setCurrentIndex(index);
    m_stack->setCurrentIndex(index);
}

void ShellWindow::onGearChanged(GearState gear, const QString &source)
{
    Q_UNUSED(source)
    if (m_vsomeipClient)
        m_vsomeipClient->publishGear(gear);

    const bool isReverse = (static_cast<quint8>(gear) == 1); // GearState::R
    if (isReverse) {
        if (!m_reverseCamera) {
            m_reverseCamera = new ReverseCameraWindow(this);
            m_reverseCamera->setAttribute(Qt::WA_DeleteOnClose);
            connect(m_reverseCamera, &QWidget::destroyed, this, [this] {
                m_reverseCamera = nullptr;
            });
            QScreen *screen = QApplication::primaryScreen();
            if (screen) {
                QRect g = screen->availableGeometry();
                m_reverseCamera->move(
                    g.x() + (g.width()  - m_reverseCamera->width())  / 2,
                    g.y() + (g.height() - m_reverseCamera->height()) / 2 - 80
                );
            }
        }
        m_reverseCamera->show();
        m_reverseCamera->raise();
    } else {
        if (m_reverseCamera) m_reverseCamera->close();
    }
}

static QString gearToDirection(GearState g)
{
    switch (static_cast<quint8>(g)) {
    case 3: return "F";
    case 1: return "R";
    default: return "N";
    }
}

void ShellWindow::pollGamepadGear()
{
    static const QString kPath = "/tmp/piracer_drive_mode.json";
    QFileInfo fi(kPath);
    if (!fi.exists()) return;
    if (fi.lastModified().msecsTo(QDateTime::currentDateTime()) > 3000) return;

    QFile f(kPath);
    if (!f.open(QIODevice::ReadOnly)) return;
    const QJsonDocument doc = QJsonDocument::fromJson(f.readAll());
    f.close();
    if (!doc.isObject()) return;

    const QString dir = doc.object().value("direction").toString().trimmed().toUpper();
    if (dir.isEmpty() || dir == m_lastFileGearDir) return;
    m_lastFileGearDir = dir;

    const GearState currentGear = m_gearStateManager->gear();
    if (dir == gearToDirection(currentGear)) return;

    GearState newGear;
    if (dir == "F")      newGear = GearState::D;
    else if (dir == "R") newGear = GearState::R;
    else                 newGear = GearState::N;

    m_gearStateManager->setGear(newGear, "gamepad");
}
