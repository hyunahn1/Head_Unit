#include "MainWindow.hpp"
#include <QGraphicsRectItem>
#include <QBrush>
#include <QPen>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    resize(900, 400);

    scene = new QGraphicsScene(this);
    view = new QGraphicsView(scene, this);
    setCentralWidget(view);
    view->setFixedSize(900, 400);

    // Draw racetrack
    scene->addRect(50, 50, 800, 300, QPen(Qt::black));

    startButton = new QPushButton("Start Race", this);
    startButton->move(10, 10);
    startButton->show();
    connect(startButton, &QPushButton::clicked, this, &MainWindow::startRace);

    // create cars
    Car* car1 = new Car("Car 1", 5);
    Car* car2 = new Car("Car 2", 7);
    Car* car3 = new Car("Car 3", 6);
    cars = {car1, car2, car3};

    // create car graphics items
    for (int i = 0; i < cars.size(); ++i)
    {
        QGraphicsRectItem* item = scene->addRect(60, 60 + i*40, 20, 20, QPen(), QBrush(Qt::red));
        carItems.append(item);

        // update graphics when car moves
        connect(cars[i], &Car::positionChanged, [item](QPointF pos){
            item->setPos(pos);
        });
    }
}

void MainWindow::startRace()
{
    for (int i = 0; i < cars.size(); ++i)
    {
        CarThread* thread = new CarThread(cars[i], this);
        threads.append(thread);

        connect(thread, &CarThread::advanceCar, cars[i], &Car::advance);
        thread->start();
    }

    startButton->setEnabled(false);
}