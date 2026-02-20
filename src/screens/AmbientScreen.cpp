/**
 * @file AmbientScreen.cpp
 * @brief Placeholder - implement after LED hardware receipt
 * @author Ahn Hyunjun
 * @date 2026-02-20
 */

#include "AmbientScreen.h"
#include "ILedController.h"
#include "GearPanel.h"
#include "GearStateManager.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>

AmbientScreen::AmbientScreen(ILedController *ledController, GearStateManager *gearState, QWidget *parent)
    : QWidget(parent)
    , m_ledController(ledController)
    , m_gearPanel(nullptr)
{
    setStyleSheet("background-color: #0D0D0F; color: #B3B3B7;");

    QHBoxLayout *root = new QHBoxLayout(this);
    root->setContentsMargins(24, 24, 0, 24);
    root->setSpacing(0);

    QVBoxLayout *content = new QVBoxLayout();
    QLabel *label = new QLabel("Ambient Lighting — Presets, Color Picker\n(Hardware TBD)", this);
    label->setAlignment(Qt::AlignCenter);
    label->setStyleSheet("font-size: 18pt; color: #6E6E73;");
    content->addWidget(label, 1);

    root->addLayout(content, 1);
    m_gearPanel = new GearPanel(gearState, this);
    root->addWidget(m_gearPanel);
}
