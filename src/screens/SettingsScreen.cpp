/**
 * @file SettingsScreen.cpp
 * @brief Placeholder - unit, theme, calibration
 * @author Ahn Hyunjun
 * @date 2026-02-20
 */

#include "SettingsScreen.h"
#include "GearPanel.h"
#include "GearStateManager.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>

SettingsScreen::SettingsScreen(GearStateManager *gearState, QWidget *parent)
    : QWidget(parent)
    , m_gearPanel(nullptr)
{
    setStyleSheet("background-color: #0D0D0F; color: #B3B3B7;");

    QHBoxLayout *root = new QHBoxLayout(this);
    root->setContentsMargins(24, 24, 0, 24);
    root->setSpacing(0);

    QVBoxLayout *content = new QVBoxLayout();
    QLabel *label = new QLabel("Settings — Unit, Theme, Calibration\n(Implement as needed)", this);
    label->setAlignment(Qt::AlignCenter);
    label->setStyleSheet("font-size: 18pt; color: #6E6E73;");
    content->addWidget(label, 1);

    root->addLayout(content, 1);
    m_gearPanel = new GearPanel(gearState, this);
    root->addWidget(m_gearPanel);
}
