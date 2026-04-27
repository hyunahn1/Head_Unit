/* ReverseCameraWindow.cpp
   FINAL VERSION
   Distance + OEM Lines + 3 Side Dots + Red Alert Blink/Vibration
*/

#include "ReverseCameraWindow.h"
<<<<<<< HEAD
#include <QtCore>
#include <QtGui>
=======
#include "PdcOverlayPainter.h"

#include <QDebug>
#include <QFont>
#include <QLinearGradient>
>>>>>>> ba084d4c6dd0091142b57ddc316ddcfc82a2794b
#include <QPainter>
#include <QFont>

ReverseCameraWindow::ReverseCameraWindow(QWidget *parent)
    : QWidget(parent)
{
    setWindowFlags(Qt::FramelessWindowHint);
    setFixedSize(640, 400);
    setAttribute(Qt::WA_TranslucentBackground);
}

ReverseCameraWindow::~ReverseCameraWindow()
{
}

<<<<<<< HEAD
void ReverseCameraWindow::setDistance(int cm)
=======
void ReverseCameraWindow::setPdcState(const PdcState &state)
{
    m_pdcState = state;
    update();
}

bool ReverseCameraWindow::startCameraPreview()
>>>>>>> ba084d4c6dd0091142b57ddc316ddcfc82a2794b
{
    if (cm < 0) cm = 0;
    if (cm > 999) cm = 999;

    m_distance = cm;
    update();
}

void ReverseCameraWindow::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    static bool blink = false;
    blink = !blink;

    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);

    const int leftPanel = 95;
    const int x0 = leftPanel;
    const int w  = width() - leftPanel;
    const int cx = x0 + w / 2;

    // ============================================
    // DISTANCE COLOR
    // ============================================
    QColor distColor;

    if (m_distance > 80)
        distColor = QColor(0,255,120);
    else if (m_distance > 40)
        distColor = QColor(255,210,0);
    else
<<<<<<< HEAD
        distColor = blink ? QColor(255,0,0) : QColor(255,80,80);

    // ============================================
    // DISTANCE BOX
    // ============================================
    p.setBrush(QColor(0,0,0,150));
    p.setPen(QPen(QColor(255,255,255,70),1));
    p.drawRoundedRect(cx-90,18,180,52,12,12);

    QFont font;
    font.setPointSize(10);
    p.setFont(font);
    p.setPen(Qt::white);
    p.drawText(QRect(cx-90,26,180,14),
               Qt::AlignCenter,
               "DISTANCE");

    font.setPointSize(20);
    font.setBold(true);
    p.setFont(font);
    p.setPen(distColor);
    p.drawText(QRect(cx-90,42,180,22),
               Qt::AlignCenter,
               QString::number(m_distance) + " cm");
=======
        p.drawPixmap(0, 0, m_placeholder);

    PdcOverlayPainter::paint(&p, rect(), m_pdcState);
}

void ReverseCameraWindow::buildPlaceholderPixmap()
{
    m_placeholder = QPixmap(640, 400);

    QPainter p(&m_placeholder);
    p.setRenderHint(QPainter::Antialiasing);
>>>>>>> ba084d4c6dd0091142b57ddc316ddcfc82a2794b

    // ============================================
    // OEM PARKING LINES
    // ============================================
    p.setPen(QPen(QColor(255,0,0),6));
    p.drawLine(cx-220,305, cx+220,305);
    p.drawLine(cx-220,305, cx-250,340);
    p.drawLine(cx+220,305, cx+250,340);

    p.setPen(QPen(QColor(255,230,0),5));
    p.drawLine(cx-170,240, cx+170,240);
    p.drawLine(cx-220,305, cx-170,240);
    p.drawLine(cx+220,305, cx+170,240);

    p.drawLine(cx-110,170, cx+110,170);
    p.drawLine(cx-170,240, cx-110,170);
    p.drawLine(cx+170,240, cx+110,170);

    p.setPen(QPen(QColor(255,255,255,180),2,Qt::DashLine));
    p.drawLine(cx,160,cx,340);

<<<<<<< HEAD
    // ============================================
    // 3 SIDE SENSOR DOTS EACH SIDE
    // ============================================
    int activeDots = 1;
=======
    font.setPointSize(10); font.setBold(false);
    p.setFont(font); p.setPen(QColor(100, 110, 120));
    p.drawText(QRect(0, 58, 640, 24), Qt::AlignCenter, "Placeholder - No camera connected");
>>>>>>> ba084d4c6dd0091142b57ddc316ddcfc82a2794b

    if (m_distance > 100) activeDots = 1;
    else if (m_distance > 60) activeDots = 2;
    else activeDots = 3;

    for (int i = 0; i < 3; i++)
    {
        QColor dotColor;

        if (i < activeDots)
            dotColor = distColor;
        else
            dotColor = QColor(80,80,80,130);

        p.setBrush(dotColor);
        p.setPen(Qt::NoPen);

        int y = 185 + i * 36;

        int offset = 0;

        // red danger vibration effect
        if (m_distance <= 40 && blink)
            offset = (i % 2 == 0) ? 2 : -2;

        p.drawEllipse(x0 + 8 + offset, y, 14, 14);
        p.drawEllipse(width() - 24 + offset, y, 14, 14);
    }

    // ============================================
    // FOOTER
    // ============================================
    font.setPointSize(9);
    font.setBold(false);
    p.setFont(font);
    p.setPen(QColor(255,255,255,220));

    p.drawText(QRect(x0,376,w,18),
               Qt::AlignCenter,
               "Check surroundings before reversing");
}