#ifndef CAR_HPP
# define CAR_HPP

#include <string>
#include <iostream>
#include "Vehicle.hpp"

class Car: public Vehicle
{
    private:
        std::string _make;
        std::string _model;
        // int _year;
    public:
        Car();
        Car(const Car& copy);
        Car& operator=(const Car& other);
        ~Car();
    	void Drive() override;
        std::string getMake();
        std::string getModel();       
};

#endif