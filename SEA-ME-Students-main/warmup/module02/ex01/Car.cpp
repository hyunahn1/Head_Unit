#include "Car.hpp"


Car::Car(): _name("Toyota Supra MK4")
{
    engine_ = new Engine();
    transmission_ = new Transmission();
    wheels_ = new Wheels();
    brakes_ = new Brake();
    lights_ = new Lights();
    steeringwheel_ = new SteeringWheel();
    frame_ = new Frame();
}

Car::~Car()
{
    delete engine_;
    delete transmission_;
    delete wheels_;
    delete brakes_;
    delete steeringwheel_;
    delete frame_;
    delete lights_;
}

void Car::PrintParts()
{
    std::cout << this->_name << std::endl;
    engine_->print();
    transmission_->print();
    wheels_->print();
    brakes_->print();
    lights_->print();
    steeringwheel_->print();
    frame_->print();
}