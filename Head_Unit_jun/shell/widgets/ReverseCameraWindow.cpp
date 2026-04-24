#include "ReverseCameraWindow.h"
#include <QPainter>
#include <QLinearGradient>
#include <QFont>

ReverseCameraWindow::ReverseCameraWindow(QWidget *parent)
    : QWidget(parent),
      m_distance(0)
{
    setWindowTitle("Rear View");
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    setFixedSize(640, 400);
    setStyleSheet("background-color: #0a0a0c;");

    // 🔁 Refresh UI periodically
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, [this]() {
        update();   // repaint
    });
    m_timer->start(50); // ~20 FPS
}

// 🔥 Called externally from CAN logic
void ReverseCameraWindow::setDistance(int distance)
{
    m_distance = distance;
}

void ReverseCameraWindow::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    // 🎨 Background gradient
    QLinearGradient grad(0, 0, 640, 400);
    grad.setColorAt(0, QColor(15, 18, 22));
    grad.setColorAt(0.5, QColor(25, 30, 35));
    grad.setColorAt(1, QColor(12, 15, 18));
    p.fillRect(rect(), grad);

    // 🎯 Dynamic grid color based on distance
    QColor gridColor;

    if (m_distance < 20)
        gridColor = QColor(255, 0, 0);       // RED
    else if (m_distance < 50)
        gridColor = QColor(255, 200, 0);     // YELLOW
    else
        gridColor = QColor(0, 200, 120);     // GREEN

    p.setPen(QPen(gridColor, 2));

    int cx = 320, cy = 200;
    int gw = 200, gh = 160;

    // 🔲 Vertical lines
    for (int i = -2; i <= 2; ++i) {
        int x = cx + i * gw / 2;
        p.drawLine(x, 80, x, 320);
    }

    // 🔲 Horizontal lines
    for (int i = -2; i <= 2; ++i) {
        int y = cy + i * gh / 2;
        p.drawLine(120, y, 520, y);
    }

    // 🎯 Center box
    p.drawRect(cx - 30, cy - 25, 60, 50);
    p.drawLine(cx - 40, cy, cx + 40, cy);
    p.drawLine(cx, cy - 35, cx, cy + 35);

    // 🚗 Distance display
    QFont font;
    font.setPointSize(24);
    font.setBold(true);
    p.setFont(font);
    p.setPen(Qt::white);

    QString distText = QString::number(m_distance) + " cm";
    p.drawText(QRect(0, 320, 640, 60), Qt::AlignCenter, distText);

    // 🏷️ Title
    font.setPointSize(14);
    p.setFont(font);
    p.setPen(QColor(0, 212, 170));
    p.drawText(QRect(0, 20, 640, 40), Qt::AlignCenter, "REAR VIEW");
}