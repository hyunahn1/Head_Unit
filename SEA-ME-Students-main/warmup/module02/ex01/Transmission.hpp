#ifndef TRANSMISSION_HPP
#define TRANSMISSION_HPP
#include "Part.hpp"

class Transmission : public Part
{
    private:
        std::string _type;
        int _gears;
        std::string _manufacturer;
        
    public:
        Transmission();
        Transmission(const Transmission& copy);
        Transmission& operator=(const Transmission& other);
        ~Transmission();
        void print() override;
};
#endif