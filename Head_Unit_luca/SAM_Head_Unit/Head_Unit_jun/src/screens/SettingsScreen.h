/**
 * @file SettingsScreen.h
 * @brief Settings tab - speed unit, about
 * @author Ahn Hyunjun
 * @date 2026-02-20
 */

#ifndef SETTINGSSCREEN_H
#define SETTINGSSCREEN_H

#include <QWidget>

class GearStateManager;
class GearPanel;

class SettingsScreen : public QWidget
{
    Q_OBJECT

public:
    explicit SettingsScreen(GearStateManager *gearState, QWidget *parent = nullptr);

signals:
    void speedUnitChanged(bool metric);   // true = km/h, false = mph

private:
    GearPanel *m_gearPanel;
};

#endif // SETTINGSSCREEN_H
