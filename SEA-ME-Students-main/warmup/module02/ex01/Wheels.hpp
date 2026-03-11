#ifndef WHEELS_HPP
#define WHEELS_HPP

#include "Part.hpp"

class Wheels : public Part
{
    private:
        std::string _name;
        int _diameter;
        std::string _width_front;
        std::string _width_rear;
        int _offset;
        std::string _material;
        std::string _bolt_pattern;
    public:
        Wheels();
        Wheels(const Wheels& copy);
        Wheels& operator=(const Wheels& other);
        ~Wheels();
        void print() override;
};
#endif