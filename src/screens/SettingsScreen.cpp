/**
 * @file SettingsScreen.cpp
 * @brief Settings tab - speed unit toggle, about info
 * @author Ahn Hyunjun
 * @date 2026-02-20
 */

#include "SettingsScreen.h"
#include "GearPanel.h"
#include "GearStateManager.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFrame>
#include <QLabel>
#include <QPushButton>

// ─────────────────────────────────────────────────────────────
static const char *kScreenStyle = R"(
    QWidget { background-color: #0D0D0F; color: #FFFFFF; }
    QLabel  { background: transparent; }
)";

static const char *kRowStyle =
    "background-color: #1A1A1E; border-radius: 12px;";

static QLabel *sectionLabel(const QString &text, QWidget *parent)
{
    QLabel *l = new QLabel(text, parent);
    l->setStyleSheet("font-size: 13pt; color: #6E6E73; margin-top: 8px;");
    return l;
}

static QFrame *rowCard(QWidget *parent)
{
    QFrame *f = new QFrame(parent);
    f->setStyleSheet(kRowStyle);
    return f;
}

// ─────────────────────────────────────────────────────────────
SettingsScreen::SettingsScreen(GearStateManager *gearState, QWidget *parent)
    : QWidget(parent)
    , m_gearPanel(nullptr)
{
    setStyleSheet(kScreenStyle);

    QHBoxLayout *root = new QHBoxLayout(this);
    root->setContentsMargins(24, 20, 0, 20);
    root->setSpacing(0);

    QVBoxLayout *content = new QVBoxLayout();
    content->setSpacing(12);

    // ── Heading ──────────────────────────────────────────────
    QLabel *heading = new QLabel("Settings", this);
    heading->setStyleSheet("font-size: 20pt; font-weight: bold; color: #FFFFFF;");
    content->addWidget(heading);

    // ── Speed Unit ───────────────────────────────────────────
    content->addWidget(sectionLabel("Display", this));

    QFrame *unitCard = rowCard(this);
    unitCard->setFixedHeight(64);
    QHBoxLayout *unitLayout = new QHBoxLayout(unitCard);
    unitLayout->setContentsMargins(20, 0, 20, 0);
    QLabel *unitText = new QLabel("Speed Unit", unitCard);
    unitText->setStyleSheet("font-size: 14pt; color: #FFFFFF;");

    QPushButton *kmhBtn = new QPushButton("km/h", unitCard);
    QPushButton *mphBtn = new QPushButton("mph",  unitCard);
    for (auto *btn : {kmhBtn, mphBtn}) {
        btn->setFixedSize(72, 36);
        btn->setCheckable(true);
    }
    const auto updateUnitStyle = [kmhBtn, mphBtn]() {
        const char *activeStyle = R"(
            QPushButton {
                background-color: #00D4AA; color: #000000;
                border: none; border-radius: 8px; font-size: 12pt; font-weight: bold;
            }
        )";
        const char *inactiveStyle = R"(
            QPushButton {
                background-color: #252529; color: #B3B3B7;
                border: none; border-radius: 8px; font-size: 12pt;
            }
            QPushButton:hover { background-color: #3A3A3F; color: #FFFFFF; }
        )";
        kmhBtn->setStyleSheet(kmhBtn->isChecked() ? activeStyle : inactiveStyle);
        mphBtn->setStyleSheet(mphBtn->isChecked() ? activeStyle : inactiveStyle);
    };

    kmhBtn->setChecked(true);
    updateUnitStyle();

    connect(kmhBtn, &QPushButton::clicked, this, [=] {
        kmhBtn->setChecked(true);
        mphBtn->setChecked(false);
        updateUnitStyle();
        emit speedUnitChanged(true);
    });
    connect(mphBtn, &QPushButton::clicked, this, [=] {
        mphBtn->setChecked(true);
        kmhBtn->setChecked(false);
        updateUnitStyle();
        emit speedUnitChanged(false);
    });

    QHBoxLayout *toggleLayout = new QHBoxLayout();
    toggleLayout->setSpacing(4);
    toggleLayout->addWidget(kmhBtn);
    toggleLayout->addWidget(mphBtn);
    unitLayout->addWidget(unitText);
    unitLayout->addStretch();
    unitLayout->addLayout(toggleLayout);
    content->addWidget(unitCard);

    // ── About ────────────────────────────────────────────────
    content->addWidget(sectionLabel("About", this));

    struct InfoRow { QString key; QString value; };
    const QList<InfoRow> infoRows = {
        { "Application",  "PiRacer Head Unit" },
        { "Version",      "1.0.0" },
        { "Build Date",   "2026-02-20" },
        { "IPC",          "VSOMEIP (peer-to-peer)" },
        { "Platform",     "Raspberry Pi 4 / Yocto Linux" },
    };

    QFrame *aboutCard = rowCard(this);
    QVBoxLayout *aboutLayout = new QVBoxLayout(aboutCard);
    aboutLayout->setContentsMargins(20, 16, 20, 16);
    aboutLayout->setSpacing(10);

    for (int i = 0; i < infoRows.size(); ++i) {
        const auto &row = infoRows[i];
        QHBoxLayout *rowLayout = new QHBoxLayout();
        QLabel *key = new QLabel(row.key, aboutCard);
        key->setStyleSheet("font-size: 13pt; color: #6E6E73;");
        key->setFixedWidth(130);
        QLabel *val = new QLabel(row.value, aboutCard);
        val->setStyleSheet("font-size: 13pt; color: #FFFFFF;");
        rowLayout->addWidget(key);
        rowLayout->addWidget(val);
        rowLayout->addStretch();
        aboutLayout->addLayout(rowLayout);

        if (i < infoRows.size() - 1) {
            QFrame *divider = new QFrame(aboutCard);
            divider->setFrameShape(QFrame::HLine);
            divider->setStyleSheet("color: #252529;");
            aboutLayout->addWidget(divider);
        }
    }
    content->addWidget(aboutCard);
    content->addStretch();

    root->addLayout(content, 1);
    m_gearPanel = new GearPanel(gearState, this);
    root->addWidget(m_gearPanel);
}
