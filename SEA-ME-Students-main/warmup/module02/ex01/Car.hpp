#ifndef CAR_HPP
#define CAR_HPP

#include "Engine.hpp"
#include "Wheels.hpp"
#include "Brake.hpp"
#include "Transmission.hpp"
#include "SteeringWheel.hpp"
#include "Lights.hpp"
#include "Frame.hpp"

class Car
{
    private:
        std::string _name;
        Engine* engine_;
        Wheels* wheels_;
        Brake* brakes_;
        Transmission* transmission_;
        Lights* lights_;
        SteeringWheel* steeringwheel_;
        Frame* frame_;
    public:
        Car();
        Car(const Car& copy);
        Car& operator=(const Car& other);
        ~Car();
        void PrintParts();
};

#endif