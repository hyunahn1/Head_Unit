#include "Wheels.hpp"

Wheels::Wheels(): _name("17-inch OEM Supra TT Wheels"), _diameter(17), _width_front("225/50R17"), _width_rear("245/40R17"), _offset(50), _material("Aluminum Alloy"), _bolt_pattern("5x114.3")
{
    std::cout << "Wheels constructor called" << std::endl;

}

Wheels::Wheels(const Wheels& copy)
{
    std::cout << "Wheels copy constructor called" << std::endl;
    *this=copy;
    return;
}

Wheels& Wheels::operator=(const Wheels& other)
{
    std::cout << "Wheels assignment operator called" << std::endl;
    if (this != &other)
    {

    }
    return (*this);
}

Wheels::~Wheels()
{
    std::cout << "Wheels deconstructor called" << std::endl;
}

void Wheels::print() 
{
    std::cout << "Wheels: " << this->_name << ", Diameter: " << this->_diameter << " inches"
              << ", Front: " << this->_width_front << ", Rear: " << this->_width_rear
              << ", Offset: +" << this->_offset << "mm, Material: " << this->_material
              << ", Bolt Pattern: " << this->_bolt_pattern << std::endl;
}