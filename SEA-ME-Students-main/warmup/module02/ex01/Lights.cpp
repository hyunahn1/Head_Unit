#include "Lights.hpp"

Lights::Lights(): _headlights("Pop-up HID"), _taillights("LED cluster"), _fog_lights(true), _type("Xenon HID")
{
    std::cout << "Lights constructor called" << std::endl;

}

Lights::Lights(const Lights& copy)
{
    std::cout << "Lights copy constructor called" << std::endl;
    *this=copy;
    return;
}

Lights& Lights::operator=(const Lights& other)
{
    std::cout << "Lights assignment operator called" << std::endl;
    if (this != &other)
    {

    }
    return (*this);
}

Lights::~Lights()
{
    std::cout << "Lights deconstructor called" << std::endl;
}

void Lights::print() 
{
    std::cout << "Lights: Headlights: " << this->_headlights << ", Taillights: " << this->_taillights
              << ", Type: " << this->_type << ", Fog Lights: " << (this->_fog_lights ? "Yes" : "No") << std::endl;
}