#include "Brake.hpp"

Brake::Brake() : _name("Brembo Xtra-Line"), _front_type("Ventilated Disc"), _front_diameter(323), _rear_diameter(320), _calipers("4-piston front, 2-piston rear"), _material("Cast Iron"), _abs(true)
{
    std::cout << "Brake constructor called" << std::endl;

}

Brake::Brake(const Brake& copy)
{
    std::cout << "Brake copy constructor called" << std::endl;
    *this=copy;
    return;
}

Brake& Brake::operator=(const Brake& other)
{
    std::cout << "Brake assignment operator called" << std::endl;
    if (this != &other)
    {

    }
    return (*this);
}

Brake::~Brake()
{
    std::cout << "Brake deconstructor called" << std::endl;
}

void Brake::print() 
{
    std::cout << "Brakes: " << this->_name << ", Type: " << this->_front_type
              << ", Front Diameter: " << this->_front_diameter << "mm"
              << ", Rear Diameter: " << this->_rear_diameter << "mm"
              << ", Calipers: " << this->_calipers << ", Material: " << this->_material
              << ", ABS: " << (this->_abs ? "Yes" : "No") << std::endl;
}