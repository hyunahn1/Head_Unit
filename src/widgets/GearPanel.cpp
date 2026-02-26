/**
 * @file GearPanel.cpp
 * @brief Sleek vertical gear panel — current gear display + P/R/N/D buttons
 * @author Ahn Hyunjun
 * @date 2026-02-20
 */

#include "GearPanel.h"
#include "GearStateManager.h"
#include "IVehicleDataProvider.h"

#include <QVBoxLayout>
#include <QPainter>
#include <QPainterPath>

// Per-gear accent colors: P, R, N, D
static constexpr const char* kColors[] = { "#48484A", "#FF3B30", "#FFD60A", "#30D158" };
// Dimmed versions for unselected state
static constexpr const char* kDimColors[] = { "#2C2C2E", "#3D1210", "#3D3100", "#0F3D1B" };

GearPanel::GearPanel(GearStateManager *gearState, QWidget *parent)
    : QWidget(parent)
    , m_gearState(gearState)
    , m_activeLabel(nullptr)
    , m_btnP(nullptr), m_btnR(nullptr), m_btnN(nullptr), m_btnD(nullptr)
    , m_neutralHoldTimer(new QTimer(this))
    , m_neutralHoldTriggered(false)
{
    setFixedWidth(80);
    setAttribute(Qt::WA_StyledBackground, true);
    setupUI();
    connect(m_gearState, &GearStateManager::gearChanged, this, &GearPanel::updateDisplay);
}

void GearPanel::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    // Dark panel background with slight rounding on left side only
    p.setPen(Qt::NoPen);
    p.setBrush(QColor(0x1C, 0x1C, 0x1E));
    p.drawRect(rect());

    // Teal left border line
    p.setBrush(QColor(0x00, 0xD4, 0xAA));
    p.drawRect(0, 0, 2, height());
}

void GearPanel::setupUI()
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(8, 20, 8, 20);
    layout->setSpacing(0);

    // ── Current gear label (large, top) ──────────────────────────
    m_activeLabel = new QLabel("P", this);
    m_activeLabel->setAlignment(Qt::AlignCenter);
    m_activeLabel->setFixedHeight(52);
    m_activeLabel->setStyleSheet(
        "font-size: 28pt; font-weight: 900; color: #00D4AA; background: transparent; letter-spacing: 2px;"
    );
    layout->addWidget(m_activeLabel);

    // Thin separator under the label
    QWidget *sep = new QWidget(this);
    sep->setFixedHeight(1);
    sep->setStyleSheet("background-color: #2C2C2E;");
    layout->addWidget(sep);
    layout->addSpacing(16);

    // ── P / R / N / D buttons ────────────────────────────────────
    auto addBtn = [this, layout](QPushButton *&btn, const char *text, int idx) {
        btn = new QPushButton(text, this);
        btn->setFixedSize(56, 56);
        btn->setProperty("gearIndex", idx);
        connect(btn, &QPushButton::clicked, this, &GearPanel::onGearButtonClicked);
        layout->addWidget(btn, 0, Qt::AlignCenter);
        layout->addSpacing(8);
    };

    addBtn(m_btnP, "P", 0);
    addBtn(m_btnR, "R", 1);
    addBtn(m_btnN, "N", 2);
    addBtn(m_btnD, "D", 3);

    m_neutralHoldTimer->setSingleShot(true);
    m_neutralHoldTimer->setInterval(600);
    connect(m_neutralHoldTimer, &QTimer::timeout, this, [this]() {
        m_neutralHoldTriggered = true;
        m_gearState->setGearFromTouch(GearState::N);
    });
    connect(m_btnN, &QPushButton::pressed, this, [this]() {
        m_neutralHoldTriggered = false;
        m_neutralHoldTimer->start();
    });
    connect(m_btnN, &QPushButton::released, this, [this]() {
        if (m_neutralHoldTimer->isActive()) {
            m_neutralHoldTimer->stop();
        }
    });

    layout->addStretch();
    updateDisplay();
}

void GearPanel::onGearButtonClicked()
{
    auto *btn = qobject_cast<QPushButton*>(sender());
    if (!btn) return;
    if (btn == m_btnN) {
        // Neutral requires hold; ignore simple click.
        return;
    }
    m_gearState->setGearFromTouch(static_cast<GearState>(btn->property("gearIndex").toInt()));
}

void GearPanel::updateDisplay()
{
    GearState g = m_gearState->gear();

    const char* letters[] = { "P", "R", "N", "D" };
    m_activeLabel->setText(letters[static_cast<int>(g)]);

    // Color the active-gear label to match the gear's accent
    m_activeLabel->setStyleSheet(QString(
        "font-size: 28pt; font-weight: 900; color: %1; background: transparent; letter-spacing: 2px;"
    ).arg(kColors[static_cast<int>(g)]));

    // Build per-button styles
    auto style = [](int idx, bool selected) -> QString {
        if (selected) {
            return QString(
                "QPushButton {"
                "  background-color: %1;"
                "  color: #FFFFFF;"
                "  font-size: 15pt; font-weight: 800;"
                "  border-radius: 12px;"
                "  border: none;"
                "}"
                "QPushButton:hover { background-color: %1; }"
            ).arg(kColors[idx]);
        } else {
            return QString(
                "QPushButton {"
                "  background-color: %1;"
                "  color: rgba(255,255,255,0.35);"
                "  font-size: 15pt; font-weight: 700;"
                "  border-radius: 12px;"
                "  border: none;"
                "}"
                "QPushButton:hover {"
                "  background-color: %2;"
                "  color: rgba(255,255,255,0.75);"
                "}"
            ).arg(kDimColors[idx]).arg(kColors[idx]);
        }
    };

    m_btnP->setStyleSheet(style(0, g == GearState::P));
    m_btnR->setStyleSheet(style(1, g == GearState::R));
    m_btnN->setStyleSheet(style(2, g == GearState::N));
    m_btnD->setStyleSheet(style(3, g == GearState::D));
}
