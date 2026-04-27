#ifndef REVERSECAMERAWINDOW_H
#define REVERSECAMERAWINDOW_H

#include <QWidget>
#include <QPaintEvent>

class ReverseCameraWindow : public QWidget
{
public:
    explicit ReverseCameraWindow(QWidget *parent = nullptr);
    ~ReverseCameraWindow() override;

    void setDistance(int cm);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    int m_distance = 150;
};

#endif // REVERSECAMERAWINDOW_H