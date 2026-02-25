/**
 * @file ReverseCameraWindow.h
 * @brief Overlay window showing placeholder reverse camera view
 *        Opens when gear is R, closes automatically when gear changes
 * @author PiRacer Head Unit
 * @date 2026-02-23
 */

#ifndef REVERSECAMERAWINDOW_H
#define REVERSECAMERAWINDOW_H

#include <QWidget>
#include <QLabel>

class ReverseCameraWindow : public QWidget
{
    Q_OBJECT

public:
    explicit ReverseCameraWindow(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    void buildPlaceholderPixmap();
    QPixmap m_placeholder;
};

#endif // REVERSECAMERAWINDOW_H
