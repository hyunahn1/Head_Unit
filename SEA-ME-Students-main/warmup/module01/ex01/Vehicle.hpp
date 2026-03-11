#ifndef VEHICLE_HPP
#define VEHICLE_HPP

#include <iostream>
#include <string>

class Vehicle
{
	protected:
		std::string _brand;
		int	_year;
	public:
		Vehicle();
		Vehicle(std::string brand, int year);
		Vehicle(const Vehicle& copy);
		Vehicle& operator=(const Vehicle& other);
		virtual ~Vehicle();
		virtual void Drive() = 0;
		int getYear() const;
		void setYear(int year);        

};

#endif