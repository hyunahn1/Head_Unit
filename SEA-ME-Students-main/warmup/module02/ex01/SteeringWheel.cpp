#include "SteeringWheel.hpp"

SteeringWheel::SteeringWheel(): _type("Leather-wrapped"), _diameter(370), _airbag(true), _power_steering("Hydraulic")
{
    std::cout << "SteeringWheel constructor called" << std::endl;

}

SteeringWheel::SteeringWheel(const SteeringWheel& copy)
{
    std::cout << "SteeringWheel copy constructor called" << std::endl;
    *this=copy;
    return;
}

SteeringWheel& SteeringWheel::operator=(const SteeringWheel& other)
{
    std::cout << "SteeringWheel assignment operator called" << std::endl;
    if (this != &other)
    {

    }
    return (*this);
}

SteeringWheel::~SteeringWheel()
{
    std::cout << "SteeringWheel deconstructor called" << std::endl;
}

void SteeringWheel::print() 
{
    std::cout << "Steering Wheel: Type: " << this->_type << ", Diameter: " << this->_diameter << "mm"
              << ", Airbag: " << (this->_airbag ? "Yes" : "No") << ", Power Steering: " << this->_power_steering << std::endl;
}