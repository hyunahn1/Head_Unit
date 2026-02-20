/**
 * @file MainWindow.cpp
 * @brief Head Unit Main Window Implementation
 * @author Ahn Hyunjun
 * @date 2026-02-20
 */

#include "MainWindow.h"
#include "widgets/TabBar.h"
#include "widgets/StatusBar.h"
#include "screens/MediaScreen.h"
#include "screens/AmbientScreen.h"
#include "screens/GearScreen.h"
#include "screens/SettingsScreen.h"
#include "ipc/MockVehicleDataProvider.h"
#include "ipc/GearStateManager.h"
#include "led/MockLedController.h"

#include <QVBoxLayout>
#include <QWidget>

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
{
    setFixedSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    setWindowTitle("PiRacer Head Unit");

    m_vehicleData = new MockVehicleDataProvider(this);
    m_gearStateManager = new GearStateManager(this);
    m_ledController = new MockLedController(this);

    setupUI();
    setupConnections();
    applyStyles();
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
    m_contentStack = new QStackedWidget(this);
    m_mediaScreen = new MediaScreen(m_gearStateManager, this);
    m_ambientScreen = new AmbientScreen(m_ledController, m_gearStateManager, this);
    m_gearScreen = new GearScreen(m_gearStateManager, this);
    m_settingsScreen = new SettingsScreen(m_gearStateManager, this);

    m_contentStack->addWidget(m_mediaScreen);
    m_contentStack->addWidget(m_ambientScreen);
    m_contentStack->addWidget(m_gearScreen);
    m_contentStack->addWidget(m_settingsScreen);

    mainLayout->addWidget(m_contentStack, 1);

    // Status bar
    m_statusBar = new StatusBar(m_vehicleData, m_gearStateManager, this);
    m_statusBar->setFixedHeight(STATUS_BAR_HEIGHT);
    mainLayout->addWidget(m_statusBar);
}

void MainWindow::setupConnections()
{
    connect(m_tabBar, &TabBar::tabSelected, this, &MainWindow::onTabChanged);
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
    if (index >= 0 && index < m_contentStack->count()) {
        m_contentStack->setCurrentIndex(index);
    }
}
