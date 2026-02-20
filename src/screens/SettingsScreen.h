/**
 * @file SettingsScreen.h
 * @brief Settings tab - calibration, theme, etc.
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

private:
    GearPanel *m_gearPanel;
};

#endif // SETTINGSSCREEN_H
