#include "Car.hpp"

Car::Car(const QString& name, int speed, QObject* parent)
    : QObject(parent), m_name(name), m_speed(speed), m_position(0, 0)
{
}

void Car::advance()
{
    // move the car horizontally for simplicity
    m_position.setX(m_position.x() + m_speed);
    emit positionChanged(m_position);
}

QPointF Car::position() const
{
    return m_position;
}

QString Car::name() const
{
    return m_name;
}