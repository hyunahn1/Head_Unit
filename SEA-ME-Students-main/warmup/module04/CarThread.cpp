#include "CarThread.hpp"
#include <QThread>

CarThread::CarThread(Car* car, QObject* parent)
    : QThread(parent), m_car(car)
{
}

void CarThread::run()
{
    // simple race: move until X reaches 800 (finish line)
    while (m_car->position().x() < 800)
    {
        QThread::msleep(50);   // 50ms per step
        emit advanceCar();
    }
}