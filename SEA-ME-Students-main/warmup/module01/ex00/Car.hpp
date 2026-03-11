#ifndef 
# define CAR_HPP

class Car
{
    private:
        std::string name;
        int speed;
    public:
        Car();
        Car(const Car& copy);
        Car& operator=(const Car& other);
        ~Car();
        
}

#endif