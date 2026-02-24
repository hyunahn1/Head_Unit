/**
 * @file AmbientScreen.cpp
 * @brief Lighting tab - presets, color picker, brightness
 *        Emits ambientColorChanged signal → MainWindow applies GlowOverlay globally
 * @author Ahn Hyunjun
 * @date 2026-02-20
 */

#include "AmbientScreen.h"
#include "ILedController.h"
#include "GearPanel.h"
#include "GearStateManager.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFrame>
#include <QColor>

// ─────────────────────────────────────────────────────────────
static const char *kScreenStyle = R"(
    QWidget { background-color: #0D0D0F; color: #FFFFFF; }
    QLabel  { background: transparent; }
    QSlider::groove:horizontal {
        background: #2C2C30; height: 6px; border-radius: 3px;
    }
    QSlider::sub-page:horizontal {
        background: #00D4AA; height: 6px; border-radius: 3px;
    }
    QSlider::handle:horizontal {
        background: #FFFFFF; width: 14px; height: 14px;
        margin: -4px 0; border-radius: 7px;
    }
)";

struct PresetDef { const char *label; const char *preset; uint8_t r, g, b; };
static const PresetDef kPresets[] = {
    { "Night",  "night",  255, 100,   0 },
    { "Day",    "day",    255, 240, 200 },
    { "Relax",  "relax",  100,  60, 200 },
    { "Sport",  "sport",  255,  20,  20 },
    { "Focus",  "focus",    0, 180, 255 },
};

struct ColorDef { const char *name; uint8_t r, g, b; };
static const ColorDef kColors[] = {
    { "White",  255, 255, 255 },
    { "Warm",   255, 200, 100 },
    { "Teal",     0, 212, 170 },
    { "Blue",     0, 100, 255 },
    { "Red",    255,  30,  30 },
    { "Purple", 160,  60, 220 },
};

// ─────────────────────────────────────────────────────────────
AmbientScreen::AmbientScreen(ILedController *ledController,
                             GearStateManager *gearState,
                             QWidget *parent)
    : QWidget(parent)
    , m_ledController(ledController)
    , m_gearPanel(nullptr)
{
    setStyleSheet(kScreenStyle);

    QHBoxLayout *root = new QHBoxLayout(this);
    root->setContentsMargins(24, 20, 0, 20);
    root->setSpacing(0);

    QVBoxLayout *content = new QVBoxLayout();
    content->setSpacing(20);

    // ── Header ───────────────────────────────────────────────
    QLabel *heading = new QLabel("Ambient Lighting", this);
    heading->setStyleSheet("font-size: 20pt; font-weight: bold; color: #FFFFFF;");
    content->addWidget(heading);

    // ── Presets ──────────────────────────────────────────────
    QLabel *presetLabel = new QLabel("Presets", this);
    presetLabel->setStyleSheet("font-size: 13pt; color: #B3B3B7;");
    content->addWidget(presetLabel);

    QHBoxLayout *presetsRow = new QHBoxLayout();
    presetsRow->setSpacing(10);
    for (const auto &p : kPresets) {
        QPushButton *btn = makePresetBtn(p.label, p.preset);
        connect(btn, &QPushButton::clicked, this, [this, p] {
            onPresetClicked(p.preset);
            applyColor(p.r, p.g, p.b);
        });
        presetsRow->addWidget(btn);
    }

    QPushButton *offBtn = new QPushButton("Off", this);
    offBtn->setFixedHeight(40);
    offBtn->setStyleSheet(R"(
        QPushButton {
            background-color: #252529; color: #6E6E73;
            border: 1px solid #3A3A3F; border-radius: 8px;
            font-size: 12pt;
        }
        QPushButton:hover { color: #FFFFFF; border-color: #FF4757; }
    )");
    connect(offBtn, &QPushButton::clicked, this, &AmbientScreen::onOffClicked);
    presetsRow->addWidget(offBtn);
    presetsRow->addStretch();
    content->addLayout(presetsRow);

    // ── Color swatches ───────────────────────────────────────
    QLabel *colorLabel = new QLabel("Colors", this);
    colorLabel->setStyleSheet("font-size: 13pt; color: #B3B3B7;");
    content->addWidget(colorLabel);

    QHBoxLayout *colorsRow = new QHBoxLayout();
    colorsRow->setSpacing(10);
    for (const auto &c : kColors) {
        QColor col(c.r, c.g, c.b);
        QPushButton *btn = makeColorBtn(col, c.name);
        uint8_t r = c.r, g = c.g, b = c.b;
        connect(btn, &QPushButton::clicked, this,
                [this, r, g, b, name = QString(c.name)] {
            onColorSwatchClicked(r, g, b, name);
        });
        colorsRow->addWidget(btn);
    }
    colorsRow->addStretch();
    content->addLayout(colorsRow);

    // ── Brightness ───────────────────────────────────────────
    QLabel *brightnessLabel = new QLabel("Brightness", this);
    brightnessLabel->setStyleSheet("font-size: 13pt; color: #B3B3B7;");
    content->addWidget(brightnessLabel);

    QHBoxLayout *brightnessRow = new QHBoxLayout();
    brightnessRow->setSpacing(12);
    QLabel *dimIcon    = new QLabel("☽", this);
    QLabel *brightIcon = new QLabel("☀", this);
    dimIcon->setStyleSheet("font-size: 14pt; color: #6E6E73;");
    brightIcon->setStyleSheet("font-size: 14pt; color: #FFFFFF;");
    m_brightnessSlider = new QSlider(Qt::Horizontal, this);
    m_brightnessSlider->setRange(0, 100);
    m_brightnessSlider->setValue(75);
    m_brightnessLabel = new QLabel("75%", this);
    m_brightnessLabel->setStyleSheet(
        "font-size: 12pt; color: #6E6E73; min-width: 36px;");
    m_brightnessLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    brightnessRow->addWidget(dimIcon);
    brightnessRow->addWidget(m_brightnessSlider, 1);
    brightnessRow->addWidget(brightIcon);
    brightnessRow->addWidget(m_brightnessLabel);
    content->addLayout(brightnessRow);

    // ── Color preview card ────────────────────────────────────
    QFrame *previewCard = new QFrame(this);
    previewCard->setStyleSheet("background-color: #1A1A1E; border-radius: 12px;");
    previewCard->setFixedHeight(100);
    QHBoxLayout *previewLayout = new QHBoxLayout(previewCard);
    previewLayout->setContentsMargins(20, 12, 20, 12);
    QLabel *previewText = new QLabel("Current Color", previewCard);
    previewText->setStyleSheet("font-size: 12pt; color: #6E6E73;");
    m_colorPreview = new QLabel(previewCard);
    m_colorPreview->setFixedSize(64, 64);
    m_colorPreview->setStyleSheet(
        "background-color: #252529; border-radius: 32px;");
    previewLayout->addWidget(previewText);
    previewLayout->addStretch();
    previewLayout->addWidget(m_colorPreview);
    content->addWidget(previewCard);

    content->addStretch();

    root->addLayout(content, 1);
    m_gearPanel = new GearPanel(gearState, this);
    root->addWidget(m_gearPanel);

    // Connections
    connect(m_brightnessSlider, &QSlider::valueChanged,
            this, &AmbientScreen::onBrightnessChanged);

    // Init LED
    if (m_ledController) {
        m_ledController->init();
        m_ledController->setBrightness(75);
    }
}

// ─────────────────────────────────────────────────────────────
QPushButton *AmbientScreen::makePresetBtn(const QString &label, const QString &)
{
    QPushButton *btn = new QPushButton(label, this);
    btn->setFixedHeight(40);
    btn->setStyleSheet(R"(
        QPushButton {
            background-color: #1A1A1E; color: #B3B3B7;
            border: 1px solid #3A3A3F; border-radius: 8px;
            font-size: 12pt; padding: 0 16px;
        }
        QPushButton:hover   { background-color: #252529; color: #FFFFFF;
                              border-color: #00D4AA; }
        QPushButton:pressed { background-color: #00D4AA22; color: #00D4AA; }
    )");
    return btn;
}

QPushButton *AmbientScreen::makeColorBtn(const QColor &color, const QString &name)
{
    QPushButton *btn = new QPushButton(this);
    btn->setFixedSize(52, 52);
    btn->setToolTip(name);
    btn->setStyleSheet(QString(R"(
        QPushButton {
            background-color: %1;
            border: 2px solid transparent; border-radius: 26px;
        }
        QPushButton:hover   { border-color: #FFFFFF; }
        QPushButton:pressed { border-color: #00D4AA; }
    )").arg(color.name()));
    return btn;
}

void AmbientScreen::applyColor(uint8_t r, uint8_t g, uint8_t b)
{
    m_r = r; m_g = g; m_b = b;
    m_colorActive = true;
    m_colorPreview->setStyleSheet(
        QString("background-color: rgb(%1,%2,%3); border-radius: 32px;")
            .arg(r).arg(g).arg(b));
    if (m_ledController) m_ledController->setColor(r, g, b);
    emit ambientColorChanged(r, g, b, m_brightness);
}

void AmbientScreen::onPresetClicked(const QString &preset)
{
    if (m_ledController) m_ledController->setPreset(preset);
}

void AmbientScreen::onColorSwatchClicked(uint8_t r, uint8_t g, uint8_t b,
                                         const QString &)
{
    applyColor(r, g, b);
}

void AmbientScreen::onBrightnessChanged(int value)
{
    m_brightness = value;
    m_brightnessLabel->setText(QString("%1%").arg(value));
    if (m_ledController) m_ledController->setBrightness(uint8_t(value));
    if (m_colorActive)
        emit ambientColorChanged(m_r, m_g, m_b, m_brightness);
}

void AmbientScreen::onOffClicked()
{
    m_colorActive = false;
    if (m_ledController) m_ledController->off();
    m_colorPreview->setStyleSheet(
        "background-color: #252529; border-radius: 32px;");
    emit ambientOff();
}
