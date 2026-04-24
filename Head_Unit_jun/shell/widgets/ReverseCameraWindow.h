#ifndef REVERSECAMERAWINDOW_H
#define REVERSECAMERAWINDOW_H

#include <QWidget>
#include <QTimer>

class ReverseCameraWindow : public QWidget
{
    Q_OBJECT

public:
    explicit ReverseCameraWindow(QWidget *parent = nullptr);

    // 🔥 Call this from your CAN module
    void setDistance(int distance);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    int m_distance;     // Distance in cm
    QTimer *m_timer;    // UI refresh timer
};

#endif // REVERSECAMERAWINDOW_H