#include "Frame.hpp"

Frame::Frame(): _chassis_code("JZA80"), _material("Steel Unibody"), _weight(1570), _wheelbase(2550), _length(4520), _width(1810), _height(1275)
{
    std::cout << "Frame constructor called" << std::endl;

}

Frame::Frame(const Frame& copy)
{
    std::cout << "Frame copy constructor called" << std::endl;
    *this=copy;
    return;
}

Frame& Frame::operator=(const Frame& other)
{
    std::cout << "Frame assignment operator called" << std::endl;
    if (this != &other)
    {

    }
    return (*this);
}

Frame::~Frame()
{
    std::cout << "Frame deconstructor called" << std::endl;
}

void Frame::print() 
{
    std::cout << "Frame: Chassis Code " << this->_chassis_code << ", Material: " << this->_material
              << ", Weight: " << this->_weight << "kg, Wheelbase: " << this->_wheelbase << "mm"
              << ", Dimensions (LxWxH): " << this->_length << "x" << this->_width << "x" << this->_height << "mm" << std::endl;
}