/**
 * @file GearScreen.h
 * @brief Gear tab — sport-style ring display + P/R/N/D selector strip
 * @author Ahn Hyunjun
 * @date 2026-02-20
 */

#ifndef GEARSCREEN_H
#define GEARSCREEN_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>

class GearStateManager;
class GearRingDisplay;   // defined in GearScreen.cpp

class GearScreen : public QWidget
{
    Q_OBJECT

public:
    explicit GearScreen(GearStateManager *gearState, QWidget *parent = nullptr);

private slots:
    void onGearButtonClicked();
    void updateDisplay();

private:
    void setupUI();

    GearStateManager   *m_gearState;
    GearRingDisplay    *m_ring;
    QLabel             *m_sourceLabel;
    QPushButton        *m_btnP;
    QPushButton        *m_btnR;
    QPushButton        *m_btnN;
    QPushButton        *m_btnD;
};

#endif // GEARSCREEN_H
