#ifndef ENGINE_HPP
#define ENGINE_HPP
#include "Part.hpp"

class Engine : public Part
{
    private:
        std::string _name;
        int _horsepower;
        int _torque;
        std::string _displacement;
        int _cylinders;
        std::string _configuration;
        std::string _turbo;
        std::string _compression_ratio;
        int _max_RPM;
    public:
        Engine();
        Engine(const Engine& copy);
        Engine& operator=(const Engine& other);
        ~Engine();
        void print() override;
};
#endif