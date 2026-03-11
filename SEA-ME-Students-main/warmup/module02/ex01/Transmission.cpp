#include "Transmission.hpp"


Transmission::Transmission(): _type("6-speed manual"), _gears(6), _manufacturer("Getrag")
{
    std::cout << "Transmission constructor called" << std::endl;

}

Transmission::Transmission(const Transmission& copy)
{
    std::cout << "Transmission copy constructor called" << std::endl;
    *this=copy;
    return;
}

Transmission& Transmission::operator=(const Transmission& other)
{
    std::cout << "Transmission assignment operator called" << std::endl;
    if (this != &other)
    {

    }
    return (*this);
}

Transmission::~Transmission()
{
    std::cout << "Transmission deconstructor called" << std::endl;
}

void Transmission::print() 
{
    std::cout <<"Transmission: " << this->_type << ", Gears: " << this->_gears << ", Manufaturer: " << this-> _manufacturer << std::endl;
} 