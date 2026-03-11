#ifndef CARTHREAD_HPP
#define CARTHREAD_HPP

#include <QThread>
#include "Car.hpp"

class CarThread : public QThread
{
    Q_OBJECT
public:
    CarThread(Car* car, QObject* parent = nullptr);

signals:
    void advanceCar();

protected:
    void run() override;

private:
    Car* m_car;
};

#endif