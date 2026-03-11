#include "Vehicle.hpp"

Vehicle::Vehicle() : _brand("Unknown"), _year(2020)
{
    std::cout << "Vehicle constructor called" << std::endl;
}

Vehicle::Vehicle(std::string brand, int year) : _brand(brand), _year(year)
{
    std::cout << "Vehicle parametric constructor called" << std::endl;
}

Vehicle::Vehicle(const Vehicle& copy)
{
    std::cout << "Vehicle copy constructor called" << std::endl;
    *this = copy;
}

Vehicle& Vehicle::operator=(const Vehicle& other)
{
    std::cout << "Vehicle assignment operator called" << std::endl;
    if (this != &other)
    {
        this->_brand = other._brand;
        this->_year = other._year;
    }
    return (*this);
}

Vehicle::~Vehicle()
{
    std::cout << "Vehicle destructor called" << std::endl;
}

int Vehicle::getYear() const
{
	return (this->_year);
}

void Vehicle::setYear(int year)
{
	this->_year = year;
}