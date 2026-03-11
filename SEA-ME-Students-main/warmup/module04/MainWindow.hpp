#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QPushButton>
#include <QGraphicsRectItem>
#include "Car.hpp"
#include "CarThread.hpp"

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget* parent = nullptr);

private slots:
    void startRace();

private:
    QGraphicsScene* scene;
    QGraphicsView* view;
    QPushButton* startButton;
    QList<Car*> cars;
    QList<QGraphicsRectItem*> carItems;
    QList<CarThread*> threads;
};

#endif