#include "ReverseCameraWindow.h"
#include "PdcOverlayPainter.h"

#include <QDebug>
#include <QFont>
#include <QLinearGradient>
#include <QPainter>

#ifdef HU_CAMERA_PREVIEW_AVAILABLE
#include <gst/gst.h>
#include <gst/app/gstappsink.h>
#endif

static constexpr int kFrameIntervalMs = 33;   // ~30 fps polling
static constexpr int kNoFrameLimit    = 90;   // ~3 s with no frame → fallback

ReverseCameraWindow::ReverseCameraWindow(QWidget *parent)
    : QWidget(parent)
{
    setWindowTitle("Rear View");
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    setFixedSize(640, 400);
    setStyleSheet("background-color: #0a0a0c;");

    buildPlaceholderPixmap();

    if (startCameraPreview()) {
        m_showPlaceholder = false;
        m_frameTimer = new QTimer(this);
        connect(m_frameTimer, &QTimer::timeout, this, &ReverseCameraWindow::onPullFrame);
        m_frameTimer->start(kFrameIntervalMs);
    }
}

ReverseCameraWindow::~ReverseCameraWindow()
{
    if (m_frameTimer) m_frameTimer->stop();
    stopCameraPreview();
}

void ReverseCameraWindow::setPdcState(const PdcState &state)
{
    m_pdcState = state;
    update();
}

bool ReverseCameraWindow::startCameraPreview()
{
#ifdef HU_CAMERA_PREVIEW_AVAILABLE
    if (!gst_is_initialized())
        gst_init(nullptr, nullptr);

    GError *err = nullptr;
    const char *pipeStr =
        "libcamerasrc ! "
        "videoconvert ! "
        "videoscale ! "
        "video/x-raw,format=RGB,width=640,height=400,framerate=30/1 ! "
        "appsink name=sink max-buffers=2 drop=true sync=false";

    GstElement *pipeline = gst_parse_launch(pipeStr, &err);
    if (!pipeline || err) {
        qWarning() << "[RearCamera] pipeline parse failed:"
                   << (err ? err->message : "unknown");
        if (err)      g_error_free(err);
        if (pipeline) gst_object_unref(pipeline);
        return false;
    }

    GstElement *appsink = gst_bin_get_by_name(GST_BIN(pipeline), "sink");
    if (!appsink) {
        qWarning() << "[RearCamera] appsink element not found";
        gst_object_unref(pipeline);
        return false;
    }

    GstStateChangeReturn ret = gst_element_set_state(pipeline, GST_STATE_PLAYING);
    if (ret == GST_STATE_CHANGE_FAILURE) {
        qWarning() << "[RearCamera] failed to set pipeline PLAYING";
        gst_object_unref(appsink);
        gst_object_unref(pipeline);
        return false;
    }

    m_pipeline = pipeline;
    m_appsink  = appsink;
    qInfo() << "[RearCamera] libcamerasrc pipeline started";
    return true;
#else
    return false;
#endif
}

void ReverseCameraWindow::stopCameraPreview()
{
#ifdef HU_CAMERA_PREVIEW_AVAILABLE
    if (m_pipeline) {
        gst_element_set_state(reinterpret_cast<GstElement *>(m_pipeline), GST_STATE_NULL);
        gst_object_unref(reinterpret_cast<GstElement *>(m_pipeline));
        m_pipeline = nullptr;
    }
    if (m_appsink) {
        gst_object_unref(reinterpret_cast<GstElement *>(m_appsink));
        m_appsink = nullptr;
    }
#endif
}

void ReverseCameraWindow::onPullFrame()
{
#ifdef HU_CAMERA_PREVIEW_AVAILABLE
    if (!m_appsink) return;

    GstSample *sample = gst_app_sink_try_pull_sample(
        GST_APP_SINK(reinterpret_cast<GstElement *>(m_appsink)), 0);

    if (!sample) {
        if (++m_noFrameCount > kNoFrameLimit) {
            qWarning() << "[RearCamera] no frames received, falling back to placeholder";
            m_showPlaceholder = true;
            m_frameTimer->stop();
            update();
        }
        return;
    }

    m_noFrameCount = 0;
    GstBuffer *buf = gst_sample_get_buffer(sample);
    if (buf) {
        GstMapInfo map;
        if (gst_buffer_map(buf, &map, GST_MAP_READ)) {
            m_frame = QImage(map.data, 640, 400, 640 * 3, QImage::Format_RGB888).copy();
            gst_buffer_unmap(buf, &map);
            update();
        }
    }
    gst_sample_unref(sample);
#endif
}

void ReverseCameraWindow::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QPainter p(this);
    if (!m_showPlaceholder && !m_frame.isNull())
        p.drawImage(0, 0, m_frame);
    else
        p.drawPixmap(0, 0, m_placeholder);

    PdcOverlayPainter::paint(&p, rect(), m_pdcState);
}

void ReverseCameraWindow::buildPlaceholderPixmap()
{
    m_placeholder = QPixmap(640, 400);

    QPainter p(&m_placeholder);
    p.setRenderHint(QPainter::Antialiasing);

    QLinearGradient grad(0, 0, 640, 400);
    grad.setColorAt(0,   QColor(15, 18, 22));
    grad.setColorAt(0.5, QColor(25, 30, 35));
    grad.setColorAt(1,   QColor(12, 15, 18));
    p.fillRect(m_placeholder.rect(), grad);

    p.setPen(QPen(QColor(0, 180, 120), 1.5));
    const int cx = 320, cy = 200, gw = 200, gh = 160;
    for (int i = -2; i <= 2; ++i) { int x = cx + i*gw/2; p.drawLine(x, 80, x, 320); }
    for (int i = -2; i <= 2; ++i) { int y = cy + i*gh/2; p.drawLine(120, y, 520, y); }

    p.setPen(QPen(QColor(0, 212, 170), 2));
    p.setBrush(Qt::NoBrush);
    p.drawRect(cx-30, cy-25, 60, 50);
    p.drawLine(cx-40, cy, cx+40, cy);
    p.drawLine(cx, cy-35, cx, cy+35);

    QFont font; font.setPointSize(18); font.setBold(true);
    p.setFont(font); p.setPen(QColor(0, 212, 170));
    p.drawText(QRect(0, 20, 640, 40), Qt::AlignCenter, "REAR VIEW");

    font.setPointSize(10); font.setBold(false);
    p.setFont(font); p.setPen(QColor(100, 110, 120));
    p.drawText(QRect(0, 58, 640, 24), Qt::AlignCenter, "Placeholder - No camera connected");

    p.setPen(Qt::NoPen); p.setBrush(QColor(40, 45, 50));
    p.drawRect(0, 340, 640, 60);
    p.end();
}
