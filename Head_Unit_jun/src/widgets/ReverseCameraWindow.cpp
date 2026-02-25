/**
 * @file ReverseCameraWindow.cpp
 * @brief Placeholder reverse camera view (stationary image)
 * @author PiRacer Head Unit
 * @date 2026-02-23
 */

#include "ReverseCameraWindow.h"
#include <QPainter>
#include <QLinearGradient>
#include <QFont>

ReverseCameraWindow::ReverseCameraWindow(QWidget *parent)
    : QWidget(parent)
{
    setWindowTitle("Rear View");
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    setFixedSize(640, 400);
    setStyleSheet("background-color: #0a0a0c;");
    buildPlaceholderPixmap();
}

void ReverseCameraWindow::buildPlaceholderPixmap()
{
    m_placeholder = QPixmap(640, 400);
    m_placeholder.fill(QColor(10, 12, 14));

    QPainter p(&m_placeholder);
    p.setRenderHint(QPainter::Antialiasing);
    p.setRenderHint(QPainter::SmoothPixmapTransform);

    // Dark gradient (night-vision / backup cam style)
    QLinearGradient grad(0, 0, 640, 400);
    grad.setColorAt(0, QColor(15, 18, 22));
    grad.setColorAt(0.5, QColor(25, 30, 35));
    grad.setColorAt(1, QColor(12, 15, 18));
    p.fillRect(m_placeholder.rect(), grad);

    // Parking guide grid (typical backup camera overlay)
    p.setPen(QPen(QColor(0, 180, 120), 1.5, Qt::SolidLine));
    const int cx = 320, cy = 200;
    const int gw = 200, gh = 160;

    // Vertical lines
    for (int i = -2; i <= 2; ++i) {
        int x = cx + i * gw / 2;
        if (x >= 0 && x <= 640)
            p.drawLine(x, 80, x, 320);
    }
    // Horizontal lines
    for (int i = -2; i <= 2; ++i) {
        int y = cy + i * gh / 2;
        if (y >= 80 && y <= 320)
            p.drawLine(120, y, 520, y);
    }

    // Center target
    p.setPen(QPen(QColor(0, 212, 170), 2, Qt::SolidLine));
    p.setBrush(Qt::NoBrush);
    p.drawRect(cx - 30, cy - 25, 60, 50);
    p.drawLine(cx - 40, cy, cx + 40, cy);
    p.drawLine(cx, cy - 35, cx, cy + 35);

    // "REAR VIEW" label at top
    QFont font;
    font.setPointSize(18);
    font.setBold(true);
    p.setFont(font);
    p.setPen(QColor(0, 212, 170));
    p.drawText(QRect(0, 20, 640, 40), Qt::AlignCenter, "REAR VIEW");

    // "No camera - placeholder" subtitle
    font.setPointSize(10);
    font.setBold(false);
    p.setFont(font);
    p.setPen(QColor(100, 110, 120));
    p.drawText(QRect(0, 58, 640, 24), Qt::AlignCenter, "Placeholder • No camera connected");

    // Simulated ground / bumper at bottom
    p.setPen(Qt::NoPen);
    p.setBrush(QColor(40, 45, 50));
    p.drawRect(0, 340, 640, 60);

    p.end();
}

void ReverseCameraWindow::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QPainter p(this);
    p.drawPixmap(0, 0, m_placeholder);
}
