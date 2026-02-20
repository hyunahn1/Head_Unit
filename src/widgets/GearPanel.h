/**
 * @file GearPanel.h
 * @brief Compact vertical P/R/N/D gear panel for non-Gear screens
 * @author Ahn Hyunjun
 * @date 2026-02-20
 */

#ifndef GEARPANEL_H
#define GEARPANEL_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>

class GearStateManager;

class GearPanel : public QWidget
{
    Q_OBJECT

public:
    explicit GearPanel(GearStateManager *gearState, QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;

private slots:
    void onGearButtonClicked();
    void updateDisplay();

private:
    void setupUI();

    GearStateManager *m_gearState;
    QLabel  *m_activeLabel;   // large current gear letter at top
    QPushButton *m_btnP;
    QPushButton *m_btnR;
    QPushButton *m_btnN;
    QPushButton *m_btnD;
};

#endif // GEARPANEL_H
