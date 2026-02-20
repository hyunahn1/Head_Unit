/**
 * @file AmbientScreen.h
 * @brief Lighting tab - LED color/brightness
 * @author Ahn Hyunjun
 * @date 2026-02-20
 */

#ifndef AMBIENTSCREEN_H
#define AMBIENTSCREEN_H

#include <QWidget>

class ILedController;
class GearStateManager;
class GearPanel;

class AmbientScreen : public QWidget
{
    Q_OBJECT

public:
    explicit AmbientScreen(ILedController *ledController, GearStateManager *gearState, QWidget *parent = nullptr);

private:
    ILedController *m_ledController;
    GearPanel *m_gearPanel;
};

#endif // AMBIENTSCREEN_H
