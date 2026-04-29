#ifndef REVERSECAMERAWINDOW_H
#define REVERSECAMERAWINDOW_H

#include "PdcTypes.h"

#include <QWidget>
#include <QPixmap>
#include <QImage>
#include <QTimer>

class ReverseCameraWindow : public QWidget
{
    Q_OBJECT

public:
    explicit ReverseCameraWindow(QWidget *parent = nullptr);
    ~ReverseCameraWindow() override;

public slots:
    void setPdcState(const PdcState &state);

protected:
    void paintEvent(QPaintEvent *event) override;

private slots:
    void onPullFrame();

private:
    void buildPlaceholderPixmap();
    bool startCameraPreview();
    void stopCameraPreview();

    QPixmap m_placeholder;
    bool    m_showPlaceholder = true;
    QImage  m_frame;
    PdcState m_pdcState;
    QTimer *m_frameTimer  = nullptr;
    int     m_noFrameCount = 0;

    // GstElement* stored as void* to keep GStreamer headers out of .h
    void *m_pipeline = nullptr;
    void *m_appsink  = nullptr;
};

#endif // REVERSECAMERAWINDOW_H
