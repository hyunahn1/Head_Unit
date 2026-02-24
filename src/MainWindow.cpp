/**
 * @file MainWindow.cpp
 * @brief Head Unit Main Window Implementation
 * @author Ahn Hyunjun
 * @date 2026-02-20
 */

#include "MainWindow.h"
#include "widgets/TabBar.h"
#include "widgets/StatusBar.h"
#include "widgets/GlowOverlay.h"
#include "widgets/SplashScreen.h"
#include "screens/MediaScreen.h"
#include "screens/AmbientScreen.h"
#include "screens/GearScreen.h"
#include "screens/SettingsScreen.h"
#include "ipc/MockVehicleDataProvider.h"
#include "ipc/GearStateManager.h"
#include "led/MockLedController.h"

#include <QVBoxLayout>
#include <QWidget>
#include <QLabel>
#include <QEasingCurve>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_tabBar(nullptr)
    , m_contentStack(nullptr)
    , m_statusBar(nullptr)
    , m_mediaScreen(nullptr)
    , m_ambientScreen(nullptr)
    , m_gearScreen(nullptr)
    , m_settingsScreen(nullptr)
    , m_vehicleData(nullptr)
    , m_gearStateManager(nullptr)
    , m_ledController(nullptr)
    , m_ambientGlow(nullptr)
{
    setFixedSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    setWindowTitle("PiRacer Head Unit");

    m_vehicleData      = new MockVehicleDataProvider(this);
    m_gearStateManager = new GearStateManager(this);
    m_ledController    = new MockLedController(this);

    setupUI();
    setupConnections();
    applyStyles();

    // ── Boot splash — covers entire window, deletes itself when done ──
    auto *splash = new SplashScreen(centralWidget());
    splash->setGeometry(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
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
    m_contentStack  = new QStackedWidget(this);
    m_mediaScreen   = new MediaScreen(m_gearStateManager, this);
    m_ambientScreen = new AmbientScreen(m_ledController, m_gearStateManager, this);
    m_gearScreen    = new GearScreen(m_gearStateManager, this);
    m_settingsScreen= new SettingsScreen(m_gearStateManager, this);

    m_contentStack->addWidget(m_mediaScreen);
    m_contentStack->addWidget(m_ambientScreen);
    m_contentStack->addWidget(m_gearScreen);
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
    m_ambientGlow->setGeometry(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    m_ambientGlow->raise();
}

void MainWindow::setupConnections()
{
    connect(m_tabBar, &TabBar::tabSelected, this, &MainWindow::onTabChanged);

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
