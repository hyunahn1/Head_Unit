#ifndef FRAME_HPP
#define FRAME_HPP
#include "Part.hpp"

class Frame : public Part
{
    private:
        std::string _chassis_code;
        std::string _material;
        int _weight;
        int _wheelbase;
        int _length;
        int _width;
        int _height;
    public:
        Frame();
        Frame(const Frame& copy);
        Frame& operator=(const Frame& other);
        ~Frame();
        void print() override;
};

#endif