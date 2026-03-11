#include "SportsCar.hpp"

SportsCar::SportsCar() : _topSpeed(100)
{
	std::cout << "SportsCar constructor called" << std::endl;
    return;
}

SportsCar::SportsCar(const SportsCar& copy)
{
	std::cout << "SportsCar copy constructor called" << std::endl;
    *this = copy;
    return;
}

SportsCar& SportsCar::operator=(const SportsCar& other)
{
	std::cout << "SportsCar assignment operator called" << std::endl;
    if (this != &other)
    {
        this->_topSpeed = other._topSpeed;
    }
    return (*this);
}

void SportsCar::Drive()
{
    Car::Drive();
    std::cout << "with a "<< this->getTopSpeed() << "kmh" << std::endl;
}

SportsCar::~SportsCar()
{
    std::cout << "SportsCar destructor called" << std::endl;
    return;
}


int SportsCar::getTopSpeed()
{
	return (this->_topSpeed);
}