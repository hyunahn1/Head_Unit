#include "Contact.hpp"

Contact::Contact() : name(""), nickname(""), phone_number("") {}
	  
bool Contact::add_contact()
{
	const std::string prompts[] = 
	{
		"Please input Name",
		"Please input Nickname",
		"Please input theire Phone Number"
	};

	std::string* fields[] = {&name, &nickname, &phone_number};
	
	std::string input;
	
	for (int i = 0; i < 3; i++)
	{
		std::cout << prompts[i] << std::endl;
		if( !std::getline(std::cin, input))
		{
			std::cerr << "An error occured" << std::endl;
			return (false);
		}
		while (input.empty())
		{
			// std::getline(std::cin, input);
			if( !std::getline(std::cin, input))
			{
				std::cerr << "An error occured" << std::endl;
				return (false);
			}

		}
		*fields[i] = input;
		
	}
	return (true);
}

std::string formatString(const std::string& str)
{
	std::string result = str;
	if (str.length() > 10)
	{
		result = str.substr(0, 9) + '.'; 
		return (result);
	}
	return (str);
}

void Contact::print_contact(int index) const
{
	std::stringstream ss;
	ss << index;
	std::string str = ss.str();

	const char *cstr = str.c_str();
	std::cout << std::right
			  << "Number: " <<  cstr << '\n'
			  << "Name: " << name << '\n'
			  << "Nickname: " << nickname << '\n'
			  << std::endl;
}

void Contact::print_contacts(int index) const
{
	std::stringstream ss;
	ss << index;
	std::string str = ss.str();

	const char *cstr = str.c_str();

	std::cout << std::right
			  << std::setw(10)  << formatString(cstr) << "|"
			  << std::setw(10) << formatString(name) << "|"
			  << std::setw(10) << formatString(nickname) << "|"
			  << std::endl;
}

std::string Contact::get_name() const
{ 
	return name;
}

std::string  Contact::get_nickname() const
{
	return nickname;
}

std::string  Contact::get_phone_number() const
{
	return phone_number;
}