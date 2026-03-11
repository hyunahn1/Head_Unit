#ifndef STEERINGWHEEL_HPP
#define STEERINGWHEEL_HPP
#include "Part.hpp"

class SteeringWheel : public Part
{
    private:
        std::string _type;
        int _diameter;
        bool _airbag;
        std::string _power_steering;
    public:
        SteeringWheel();
        SteeringWheel(const SteeringWheel& copy);
        SteeringWheel& operator=(const SteeringWheel& other);
        ~SteeringWheel();
        void print() override;
};
#endif