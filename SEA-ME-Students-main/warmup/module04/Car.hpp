#ifndef CAR_HPP
#define CAR_HPP

#include <QObject>
#include <QPointF>
#include <QString>

class Car : public QObject
{
    Q_OBJECT
public:
    Car(const QString& name, int speed, QObject* parent = nullptr);

    void advance();            // move car along the track
    QPointF position() const;  // current position of the car
    QString name() const;      // car's name

signals:
    void positionChanged(QPointF newPos);

private:
    QString m_name;
    int m_speed;
    QPointF m_position;
};

#endif