/**
 * @file MainWindow.cpp
 * @brief Head Unit Main Window Implementation
 * @author Ahn Hyunjun
 * @date 2026-02-20
 */

#include "MainWindow.h"
#include "widgets/TabBar.h"
#include "widgets/StatusBar.h"
#include "widgets/ReverseCameraWindow.h"
#include "widgets/GlowOverlay.h"
#include "widgets/SplashScreen.h"
#include "screens/MediaScreen.h"
#ifndef HU_PRE_API
#include "screens/YouTubeScreen.h"
#endif
#include "screens/CallScreen.h"
#include "screens/NavigationScreen.h"
#include "screens/AmbientScreen.h"
#include "screens/SettingsScreen.h"
#include "ipc/MockVehicleDataProvider.h"
#include "ipc/GearStateManager.h"
#include "ipc/IVehicleDataProvider.h"
#include "led/MockLedController.h"

#include <QVBoxLayout>
#include <QWidget>
#include <QLabel>
#include <QEasingCurve>
#include <QApplication>
#include <QScreen>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_tabBar(nullptr)
    , m_contentStack(nullptr)
    , m_statusBar(nullptr)
    , m_mediaScreen(nullptr)
#ifndef HU_PRE_API
    , m_youtubeScreen(nullptr)
    , m_youtubePlaceholder(nullptr)
#endif
    , m_callScreen(nullptr)
    , m_navigationScreen(nullptr)
#ifdef HU_WEBENGINE_AVAILABLE
    , m_navigationPlaceholder(nullptr)
#endif
    , m_ambientScreen(nullptr)
    , m_settingsScreen(nullptr)
    , m_vehicleData(nullptr)
    , m_gearStateManager(nullptr)
    , m_ledController(nullptr)
    , m_ambientGlow(nullptr)
    , m_reverseCamera(nullptr)
    , m_windowWidth(WINDOW_WIDTH_DEFAULT)
    , m_windowHeight(WINDOW_HEIGHT_DEFAULT)
{
    // 고정 해상도 1024×600 — 모든 UI가 이 비율 기준으로 설계됨
    setFixedSize(m_windowWidth, m_windowHeight);
    setWindowTitle("PiRacer Head Unit");

    m_vehicleData      = new MockVehicleDataProvider(this);
    m_gearStateManager = new GearStateManager(this);
    m_ledController    = new MockLedController(this);

    setupUI();
    setupConnections();
    applyStyles();

    // ── Boot splash — covers entire window, deletes itself when done ──
    auto *splash = new SplashScreen(centralWidget());
    splash->setGeometry(0, 0, m_windowWidth, m_windowHeight);
    splash->raise();
    connect(splash, &SplashScreen::finished, splash, &QWidget::deleteLater);
    connect(splash, &SplashScreen::finished, this, [this] { m_ambientGlow->raise(); });
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupUI()
{
    QWidget *central = new QWidget(this);
    setCentralWidget(central);

    QVBoxLayout *mainLayout = new QVBoxLayout(central);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // Tab bar
    m_tabBar = new TabBar(this);
    m_tabBar->setFixedHeight(TAB_BAR_HEIGHT);
    mainLayout->addWidget(m_tabBar);

    // Content stack (screens)
    // YouTube/Navigation WebEngine은 지연 생성 — 앱 시작 시 Chromium 로드 방지
    m_contentStack  = new QStackedWidget(this);
    m_mediaScreen   = new MediaScreen(m_gearStateManager, this);
#ifndef HU_PRE_API
    m_youtubePlaceholder = new QWidget(this);
    m_youtubePlaceholder->setStyleSheet("background:#0D0D0F;");
    auto *ytLabel = new QLabel("YouTube\n\n탭을 클릭하여 로드", m_youtubePlaceholder);
    ytLabel->setAlignment(Qt::AlignCenter);
    ytLabel->setStyleSheet("color:#6E6E73; font-size:14pt;");
    auto *ytLayout = new QVBoxLayout(m_youtubePlaceholder);
    ytLayout->addWidget(ytLabel);
#endif
    m_callScreen    = new CallScreen(m_gearStateManager, this);
#ifdef HU_WEBENGINE_AVAILABLE
    m_navigationPlaceholder = new QWidget(this);
    m_navigationPlaceholder->setStyleSheet("background:#0D0D0F;");
    auto *navLabel = new QLabel("Navigation\n\n탭을 클릭하여 지도 로드", m_navigationPlaceholder);
    navLabel->setAlignment(Qt::AlignCenter);
    navLabel->setStyleSheet("color:#6E6E73; font-size:14pt;");
    auto *navLayout = new QVBoxLayout(m_navigationPlaceholder);
    navLayout->addWidget(navLabel);
#else
    m_navigationScreen = new NavigationScreen(m_gearStateManager, this);
#endif
    m_ambientScreen = new AmbientScreen(m_ledController, m_gearStateManager, this);
    m_settingsScreen= new SettingsScreen(m_gearStateManager, this);

    m_contentStack->addWidget(m_mediaScreen);
#ifndef HU_PRE_API
    m_contentStack->addWidget(m_youtubePlaceholder);
#endif
    m_contentStack->addWidget(m_callScreen);
#ifdef HU_WEBENGINE_AVAILABLE
    m_contentStack->addWidget(m_navigationPlaceholder);
#else
    m_contentStack->addWidget(m_navigationScreen);
#endif
    m_contentStack->addWidget(m_ambientScreen);
    m_contentStack->addWidget(m_settingsScreen);

    mainLayout->addWidget(m_contentStack, 1);

    // Status bar
    m_statusBar = new StatusBar(m_vehicleData, m_gearStateManager, this);
    m_statusBar->setFixedHeight(STATUS_BAR_HEIGHT);
    mainLayout->addWidget(m_statusBar);

    // ── Global ambient glow overlay ───────────────────────────
    // Sits on top of everything; covers the full central widget.
    // Window is fixed size, so geometry is set once here.
    m_ambientGlow = new GlowOverlay(central);
    m_ambientGlow->setGeometry(0, 0, m_windowWidth, m_windowHeight);
    m_ambientGlow->raise();
}

void MainWindow::setupConnections()
{
    connect(m_tabBar, &TabBar::tabSelected, this, &MainWindow::onTabChanged);

    // Gear R → show reverse camera; any other gear → close it
    connect(m_gearStateManager, &GearStateManager::gearChanged,
            this, &MainWindow::onGearChanged);

    // Ambient color signal → update global glow on all tabs
    connect(m_ambientScreen, &AmbientScreen::ambientColorChanged,
            this, [this](uint8_t r, uint8_t g, uint8_t b, int brightness) {
        m_ambientGlow->setGlow(r, g, b, brightness);
    });
    connect(m_ambientScreen, &AmbientScreen::ambientOff,
            this, [this] {
        m_ambientGlow->clearGlow();
    });
}

void MainWindow::onGearChanged()
{
    const bool isReverse = (m_gearStateManager->gear() == GearState::R);
    if (isReverse) {
        if (!m_reverseCamera) {
            m_reverseCamera = new ReverseCameraWindow(this);
            m_reverseCamera->setAttribute(Qt::WA_DeleteOnClose);
            connect(m_reverseCamera, &QWidget::destroyed, this, [this] {
                m_reverseCamera = nullptr;
            });
            // Position near main window (or centered on screen)
            QScreen *screen = QApplication::primaryScreen();
            if (screen) {
                QRect geo = screen->availableGeometry();
                int x = geo.x() + (geo.width() - m_reverseCamera->width()) / 2;
                int y = geo.y() + (geo.height() - m_reverseCamera->height()) / 2 - 80;
                m_reverseCamera->move(x, y);
            }
        }
        m_reverseCamera->show();
        m_reverseCamera->raise();
        // activateWindow() 제거 — Pi에서 포커스 강제 시 디스플레이 끊김 가능
    } else {
        if (m_reverseCamera) {
            m_reverseCamera->close();
        }
    }
}

#ifndef HU_PRE_API
void MainWindow::ensureYouTubeScreen()
{
    if (m_youtubeScreen) return;
    const int idx = m_contentStack->indexOf(m_youtubePlaceholder);
    m_contentStack->removeWidget(m_youtubePlaceholder);
    m_youtubePlaceholder->deleteLater();
    m_youtubePlaceholder = nullptr;
    m_youtubeScreen = new YouTubeScreen(m_gearStateManager, this);
    m_contentStack->insertWidget(idx, m_youtubeScreen);
}

void MainWindow::releaseYouTubeScreen()
{
    if (!m_youtubeScreen) return;
    const int idx = m_contentStack->indexOf(m_youtubeScreen);
    m_contentStack->removeWidget(m_youtubeScreen);
    m_youtubeScreen->deleteLater();
    m_youtubeScreen = nullptr;
    m_youtubePlaceholder = new QWidget(this);
    m_youtubePlaceholder->setStyleSheet("background:#0D0D0F;");
    auto *ytLabel = new QLabel("YouTube\n\n탭을 클릭하여 로드", m_youtubePlaceholder);
    ytLabel->setAlignment(Qt::AlignCenter);
    ytLabel->setStyleSheet("color:#6E6E73; font-size:14pt;");
    auto *ytLayout = new QVBoxLayout(m_youtubePlaceholder);
    ytLayout->addWidget(ytLabel);
    m_contentStack->insertWidget(idx, m_youtubePlaceholder);
}
#endif

#ifdef HU_WEBENGINE_AVAILABLE
void MainWindow::ensureNavigationScreen()
{
    if (m_navigationScreen) return;
    const int idx = m_contentStack->indexOf(m_navigationPlaceholder);
    m_contentStack->removeWidget(m_navigationPlaceholder);
    m_navigationPlaceholder->deleteLater();
    m_navigationPlaceholder = nullptr;
    m_navigationScreen = new NavigationScreen(m_gearStateManager, this);
    m_contentStack->insertWidget(idx, m_navigationScreen);
}

void MainWindow::releaseNavigationScreen()
{
    if (!m_navigationScreen) return;
    const int idx = m_contentStack->indexOf(m_navigationScreen);
    m_contentStack->removeWidget(m_navigationScreen);
    m_navigationScreen->deleteLater();
    m_navigationScreen = nullptr;
    m_navigationPlaceholder = new QWidget(this);
    m_navigationPlaceholder->setStyleSheet("background:#0D0D0F;");
    auto *navLabel = new QLabel("Navigation\n\n탭을 클릭하여 지도 로드", m_navigationPlaceholder);
    navLabel->setAlignment(Qt::AlignCenter);
    navLabel->setStyleSheet("color:#6E6E73; font-size:14pt;");
    auto *navLayout = new QVBoxLayout(m_navigationPlaceholder);
    navLayout->addWidget(navLabel);
    m_contentStack->insertWidget(idx, m_navigationPlaceholder);
}
#endif

void MainWindow::applyStyles()
{
    setStyleSheet(
        "QMainWindow { background-color: #0D0D0F; }"
        "QWidget { background-color: #0D0D0F; color: #FFFFFF; }"
    );
}

void MainWindow::onTabChanged(int index)
{
    if (index < 0 || index >= m_contentStack->count()) return;
    if (index == m_contentStack->currentIndex()) return;
    if (m_animating) return;

    const int currentIdx = m_contentStack->currentIndex();

#ifndef HU_PRE_API
    // YouTube 탭 떠날 때 WebEngine 해제 (Chromium 프로세스 종료)
    if (currentIdx == 1 && index != 1) releaseYouTubeScreen();
    // YouTube 탭 들어갈 때 로드
    if (index == 1) ensureYouTubeScreen();
#endif
#ifdef HU_WEBENGINE_AVAILABLE
    // Navigation 탭 떠날 때 WebEngine 해제
    if (currentIdx == 3 && index != 3) releaseNavigationScreen();
    // Navigation 탭 들어갈 때 로드
    if (index == 3) ensureNavigationScreen();
#endif

    // QWidget::grab() on QWebEngineView causes crash/freeze - skip animation for WebEngine tabs
#ifdef HU_WEBENGINE_AVAILABLE
    const int kYouTubeIdx = 1, kNavigationIdx = 3;
    const bool involvesWebEngine = (index == kYouTubeIdx || index == kNavigationIdx
                                    || m_contentStack->currentIndex() == kYouTubeIdx
                                    || m_contentStack->currentIndex() == kNavigationIdx);
    if (involvesWebEngine) {
        m_contentStack->setCurrentIndex(index);
        return;
    }
#endif

    m_animating = true;
    const bool forward   = index > m_contentStack->currentIndex();
    const QRect stackRect = m_contentStack->geometry(); // relative to centralWidget()
    const int   w         = stackRect.width();
    const int   h         = stackRect.height();
    QWidget    *central   = centralWidget();

    // ── 1. Grab snapshot of the current (outgoing) page ──────────
    QPixmap oldSnap = m_contentStack->grab();

    // ── 2. Switch the stack (new page is now rendered underneath) ─
    m_contentStack->setCurrentIndex(index);

    // ── 3. Grab snapshot of the incoming page ────────────────────
    QPixmap newSnap = m_contentStack->grab();

    // ── 4. Overlay labels on top of everything ────────────────────
    auto *outLabel = new QLabel(central);
    outLabel->setPixmap(oldSnap);
    outLabel->setGeometry(stackRect);
    outLabel->show();
    outLabel->raise();

    const QRect offRight(stackRect.x() + w, stackRect.y(), w, h);
    const QRect offLeft (stackRect.x() - w, stackRect.y(), w, h);

    auto *inLabel = new QLabel(central);
    inLabel->setPixmap(newSnap);
    inLabel->setGeometry(forward ? offRight : offLeft);
    inLabel->show();
    inLabel->raise();

    // ── 5. Parallel slide animation ───────────────────────────────
    auto *animOut = new QPropertyAnimation(outLabel, "geometry", this);
    animOut->setDuration(300);
    animOut->setStartValue(stackRect);
    animOut->setEndValue(forward ? offLeft : offRight);
    animOut->setEasingCurve(QEasingCurve::OutCubic);

    auto *animIn = new QPropertyAnimation(inLabel, "geometry", this);
    animIn->setDuration(300);
    animIn->setStartValue(forward ? offRight : offLeft);
    animIn->setEndValue(stackRect);
    animIn->setEasingCurve(QEasingCurve::OutCubic);

    auto *group = new QParallelAnimationGroup(this);
    group->addAnimation(animOut);
    group->addAnimation(animIn);

    connect(group, &QParallelAnimationGroup::finished, this, [=]() {
        outLabel->deleteLater();
        inLabel->deleteLater();
        m_ambientGlow->raise();
        m_animating = false;
        group->deleteLater();
    });

    group->start();
}
