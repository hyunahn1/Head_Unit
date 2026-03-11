#include "Engine.hpp"

Engine::Engine(): _name("2JZ-GE"), _horsepower(320), _torque(431), _displacement("3.0L"), _cylinders(6), _configuration("Inline-6"), _turbo("Twin-Sequential Turbo"), _compression_ratio("8.5:1"), _max_RPM(6800)
{
    std::cout << "Engine constructor called" << std::endl;

}

Engine::Engine(const Engine& copy)
{
    std::cout << "Engine copy constructor called" << std::endl;
    *this=copy;
    return;
}

Engine& Engine::operator=(const Engine& other)
{
    std::cout << "Engine assignment operator called" << std::endl;
    if (this != &other)
    {

    }
    return (*this);
}

Engine::~Engine()
{
    std::cout << "Engine deconstructor called" << std::endl;
}

void Engine::print() 
{
    std::cout << "Engine: " << this->_name << ", Horsepower: " << this->_horsepower 
              << " HP, Torque: " << this->_torque << " NM, Displacement: " << this->_displacement 
              << ", Cylinders: " << this->_cylinders << ", Configuration: " << this->_configuration 
              << ", Turbo: " << this->_turbo << ", Compression-Ratio: " << this->_compression_ratio 
              << ", Max_RPM: " << this->_max_RPM << std::endl;
}