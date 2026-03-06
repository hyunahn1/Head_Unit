/**
 * @file CallScreen.h
 * @brief Call tab - phone call interface
 */

#ifndef CALLSCREEN_H
#define CALLSCREEN_H

#include <QWidget>

class GearStateManager;
class GearPanel;

class CallScreen : public QWidget
{
    Q_OBJECT

public:
    explicit CallScreen(GearStateManager *gearState, QWidget *parent = nullptr);

private slots:
    void onCallClicked();
    void onEndCallClicked();

private:
    void setupUI(GearStateManager *gearState);

    GearPanel *m_gearPanel;
    bool m_inCall = false;
};

#endif // CALLSCREEN_H
