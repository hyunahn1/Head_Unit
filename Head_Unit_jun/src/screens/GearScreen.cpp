/**
 * @file GearScreen.cpp
 * @brief Sport-style gear selector — arc ring display + pill strip
 * @author Ahn Hyunjun
 * @date 2026-02-20
 */

#include "GearScreen.h"
#include "GearStateManager.h"
#include "IVehicleDataProvider.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPainter>
#include <QPainterPath>
#include <QFont>

// ─── Gear palette ──────────────────────────────────────────────────────────
struct GearStyle { QColor color; QColor dim; const char *letter; };
static const GearStyle kG[] = {
    { QColor("#8E8E93"), QColor("#28282A"), "P" },  // Park    — silver
    { QColor("#FF3B30"), QColor("#3A0A08"), "R" },  // Reverse — red
    { QColor("#FFD60A"), QColor("#3A3000"), "N" },  // Neutral — amber
    { QColor("#30D158"), QColor("#0A2F14"), "D" },  // Drive   — green
};

// ─── Ring widget (custom painted) ─────────────────────────────────────────
class GearRingDisplay : public QWidget
{
public:
    explicit GearRingDisplay(GearStateManager *gs, QWidget *parent = nullptr)
        : QWidget(parent), m_gs(gs)
    {
        setFixedSize(240, 240);
        setAttribute(Qt::WA_OpaquePaintEvent, false);
    }
    void refresh() { update(); }

protected:
    void paintEvent(QPaintEvent *) override
    {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);

        int idx = static_cast<int>(m_gs->gear());
        QColor gc = kG[idx].color;

        const int W = width(), H = height();
        const int margin = 16;
        QRectF outerRect(margin, margin, W - 2*margin, H - 2*margin);
        QRectF arcRect = outerRect.adjusted(6, 6, -6, -6);

        // ── Dark circle background ───────────────────────────────────────
        p.setPen(Qt::NoPen);
        QRadialGradient bg(outerRect.center(), outerRect.width() / 2.0);
        bg.setColorAt(0.0, QColor("#232326"));
        bg.setColorAt(1.0, QColor("#111113"));
        p.setBrush(bg);
        p.drawEllipse(outerRect);

        // ── Gray track (full 270° arc, gap at bottom) ────────────────────
        {
            QPen track(QColor("#2A2A2D"), 9, Qt::SolidLine, Qt::RoundCap);
            p.setPen(track);
            p.setBrush(Qt::NoBrush);
            p.drawArc(arcRect, 225 * 16, -270 * 16);
        }

        // ── Outer glow layers ────────────────────────────────────────────
        for (int layer = 3; layer >= 1; --layer) {
            QColor glow = gc;
            glow.setAlpha(layer * 18);
            QPen gp(glow, 9 + layer * 5, Qt::SolidLine, Qt::RoundCap);
            p.setPen(gp);
            p.drawArc(arcRect, 225 * 16, -270 * 16);
        }

        // ── Main arc ─────────────────────────────────────────────────────
        {
            QPen arcPen(gc, 9, Qt::SolidLine, Qt::RoundCap);
            p.setPen(arcPen);
            p.drawArc(arcRect, 225 * 16, -270 * 16);
        }

        // ── Thin inner rim ───────────────────────────────────────────────
        {
            QPen rim(QColor(0xFF, 0xFF, 0xFF, 12), 1);
            p.setPen(rim);
            p.drawEllipse(outerRect.adjusted(3, 3, -3, -3));
        }

        // ── Gear letter ──────────────────────────────────────────────────
        QFont f;
        f.setPointSize(68);
        f.setWeight(QFont::Black);
        f.setLetterSpacing(QFont::AbsoluteSpacing, -2);
        p.setFont(f);

        // Soft shadow
        QColor shadow = gc;
        shadow.setAlpha(50);
        p.setPen(shadow);
        QRectF shifted = outerRect.translated(2, 3);
        p.drawText(shifted, Qt::AlignCenter, kG[idx].letter);

        // Main letter
        p.setPen(gc);
        p.drawText(outerRect, Qt::AlignCenter, kG[idx].letter);

        // ── "GEAR" label ─────────────────────────────────────────────────
        QFont small;
        small.setPointSize(8);
        small.setWeight(QFont::Medium);
        small.setLetterSpacing(QFont::AbsoluteSpacing, 3);
        p.setFont(small);
        QColor labelColor(0xFF, 0xFF, 0xFF, 60);
        p.setPen(labelColor);
        QRectF labelRect = outerRect.adjusted(0, outerRect.height() * 0.52, 0, 0);
        p.drawText(labelRect, Qt::AlignCenter, "GEAR");
    }

private:
    GearStateManager *m_gs;
};

// ─── GearScreen ────────────────────────────────────────────────────────────
GearScreen::GearScreen(GearStateManager *gearState, QWidget *parent)
    : QWidget(parent)
    , m_gearState(gearState)
    , m_ring(nullptr)
    , m_sourceLabel(nullptr)
    , m_btnP(nullptr), m_btnR(nullptr), m_btnN(nullptr), m_btnD(nullptr)
    , m_neutralHoldTimer(new QTimer(this))
    , m_neutralHoldTriggered(false)
{
    setStyleSheet("background-color: #0D0D0F;");
    setupUI();
    connect(m_gearState, &GearStateManager::gearChanged, this, &GearScreen::updateDisplay);
}

void GearScreen::setupUI()
{
    QVBoxLayout *vbox = new QVBoxLayout(this);
    vbox->setContentsMargins(40, 24, 40, 40);
    vbox->setSpacing(0);

    // ── Ring ──────────────────────────────────────────────────────────────
    m_ring = new GearRingDisplay(m_gearState, this);
    vbox->addStretch(3);
    vbox->addWidget(m_ring, 0, Qt::AlignCenter);

    // ── Source label ──────────────────────────────────────────────────────
    m_sourceLabel = new QLabel(this);
    m_sourceLabel->setAlignment(Qt::AlignCenter);
    m_sourceLabel->setStyleSheet(
        "font-size: 10pt; color: #48484A; letter-spacing: 2px; background: transparent;"
    );
    vbox->addSpacing(14);
    vbox->addWidget(m_sourceLabel);
    vbox->addStretch(3);

    // ── Selector strip container ──────────────────────────────────────────
    QWidget *stripContainer = new QWidget(this);
    stripContainer->setFixedHeight(68);
    stripContainer->setStyleSheet(
        "background-color: #1C1C1E; border-radius: 16px;"
    );

    QHBoxLayout *strip = new QHBoxLayout(stripContainer);
    strip->setContentsMargins(12, 8, 12, 8);
    strip->setSpacing(8);

    auto addBtn = [this, strip](QPushButton *&btn, int idx) {
        btn = new QPushButton(kG[idx].letter, this);
        btn->setFixedHeight(52);
        btn->setProperty("gearIndex", idx);
        connect(btn, &QPushButton::clicked, this, &GearScreen::onGearButtonClicked);
        strip->addWidget(btn, 1);
    };
    addBtn(m_btnP, 0);
    addBtn(m_btnR, 1);
    addBtn(m_btnN, 2);
    addBtn(m_btnD, 3);

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

    vbox->addWidget(stripContainer);

    updateDisplay();
}

void GearScreen::onGearButtonClicked()
{
    auto *btn = qobject_cast<QPushButton*>(sender());
    if (!btn) return;
    if (btn == m_btnN) {
        // Neutral requires hold; ignore simple click.
        return;
    }
    m_gearState->setGearFromTouch(static_cast<GearState>(btn->property("gearIndex").toInt()));
}

void GearScreen::updateDisplay()
{
    m_ring->refresh();

    GearState g = m_gearState->gear();

    // Source label
    QString src = m_gearState->lastSource();
    src[0] = src[0].toUpper();
    m_sourceLabel->setText(QString("SOURCE  ·  %1").arg(src.toUpper()));

    // Pill button styles
    auto style = [](int idx, bool sel) -> QString {
        QColor c  = kG[idx].color;
        QColor d  = kG[idx].dim;
        if (sel) {
            return QString(
                "QPushButton {"
                "  background-color: %1;"
                "  color: #FFFFFF;"
                "  font-size: 17pt; font-weight: 900;"
                "  border-radius: 10px;"
                "  border: none;"
                "  letter-spacing: 1px;"
                "}"
            ).arg(c.name());
        } else {
            return QString(
                "QPushButton {"
                "  background-color: %1;"
                "  color: %2;"
                "  font-size: 17pt; font-weight: 700;"
                "  border-radius: 10px;"
                "  border: none;"
                "}"
                "QPushButton:hover {"
                "  background-color: %3;"
                "  color: rgba(255,255,255,180);"
                "}"
            ).arg(d.name())
             .arg(c.lighter(130).name())
             .arg(c.darker(160).name());
        }
    };

    m_btnP->setStyleSheet(style(0, g == GearState::P));
    m_btnR->setStyleSheet(style(1, g == GearState::R));
    m_btnN->setStyleSheet(style(2, g == GearState::N));
    m_btnD->setStyleSheet(style(3, g == GearState::D));
}
