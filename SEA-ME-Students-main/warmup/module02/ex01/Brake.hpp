#ifndef Brake_HPP
#define Brake_HPP

#include "Part.hpp"

class Brake: public Part
{
    private: 
        std::string _name;
        std::string _front_type;
        int _front_diameter;
        int _rear_diameter;
        std::string _calipers;
        std::string _material;
        bool _abs;
    public:
        Brake();
        Brake(const Brake& copy);
        Brake& operator=(const Brake& other);
        ~Brake();
        void print() override;
};
#endif