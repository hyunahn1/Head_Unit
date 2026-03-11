#ifndef LIGHTS_HPP
#define LIGHTS_HPP
#include "Part.hpp"

class Lights : public Part
{
    private:
        std::string _headlights;
        std::string _taillights;
        bool _fog_lights;
        std::string _type;
    public:
        Lights();
        Lights(const Lights& copy);
        Lights& operator=(const Lights& other);
        ~Lights();
        void print() override;
};
#endif