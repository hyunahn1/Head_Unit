#include "Car.hpp"

Car::Car() : Vehicle("VW", 2015), _make("VW"), _model("Passat")
{
	std::cout << "Car constructor called" << std::endl;
    return;
}

Car::Car(const Car& copy) : Vehicle(copy)
{
	std::cout << "Car copy constructor called" << std::endl;
    *this = copy;
    return;
}

Car& Car::operator=(const Car& other)
{
	std::cout << "Car assignment operator called" << std::endl;
    if (this != &other)
    {
        Vehicle::operator=(other);
        this->_make = other._make;
        this->_model = other._model;
    }
    return (*this);
}

void Car::Drive()
{
    std::cout << "Driving " << this->_make << " " << this->_model << " " << this->_year << std::endl;
    return;
}

Car::~Car()
{
    std::cout << "Car destructor called" << std::endl;
    return;
}


std::string Car::getMake()
{
	return (this->_make);
}	

std::string Car::getModel()
{
	return (this->_model);
}