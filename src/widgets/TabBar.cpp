/**
 * @file TabBar.cpp
 * @brief 4-tab bar implementation
 * @author Ahn Hyunjun
 * @date 2026-02-20
 */

#include "TabBar.h"
#include <QSignalMapper>

TabBar::TabBar(QWidget *parent)
    : QWidget(parent)
    , m_currentIndex(0)
{
    setupUI();
}

void TabBar::setupUI()
{
    m_layout = new QHBoxLayout(this);
    m_layout->setContentsMargins(24, 0, 24, 0);
    m_layout->setSpacing(0);

    const QStringList labels = { "Media", "Lighting", "Gear", "Settings" };

    for (int i = 0; i < labels.size(); ++i) {
        QPushButton *btn = new QPushButton(labels[i], this);
        btn->setMinimumSize(88, 48);
        btn->setProperty("tabIndex", i);
        btn->setCursor(Qt::PointingHandCursor);

        connect(btn, &QPushButton::clicked, this, [this, i]() {
            setCurrentIndex(i);
            emit tabSelected(i);
        });

        m_tabs.append(btn);
        m_layout->addWidget(btn);
    }

    m_layout->addStretch();
    updateTabStyles();
}

void TabBar::setCurrentIndex(int index)
{
    if (index < 0 || index >= m_tabs.size()) return;
    m_currentIndex = index;
    updateTabStyles();
}

void TabBar::onTabClicked()
{
    QPushButton *btn = qobject_cast<QPushButton*>(sender());
    if (btn && btn->property("tabIndex").isValid()) {
        int idx = btn->property("tabIndex").toInt();
        setCurrentIndex(idx);
        emit tabSelected(idx);
    }
}

void TabBar::updateTabStyles()
{
    QString selectedStyle =
        "QPushButton {"
        "  color: #00D4AA;"
        "  background: transparent;"
        "  border: none;"
        "  border-bottom: 2px solid #00D4AA;"
        "  font-weight: 600;"
        "  font-size: 14pt;"
        "}"
        "QPushButton:hover { background-color: #252529; }";
    QString normalStyle =
        "QPushButton {"
        "  color: #B3B3B7;"
        "  background: transparent;"
        "  border: none;"
        "  font-size: 14pt;"
        "}"
        "QPushButton:hover { background-color: #252529; color: #FFFFFF; }";

    for (int i = 0; i < m_tabs.size(); ++i) {
        m_tabs[i]->setStyleSheet(i == m_currentIndex ? selectedStyle : normalStyle);
    }
}
