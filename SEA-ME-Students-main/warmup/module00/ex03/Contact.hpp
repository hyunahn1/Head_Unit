#ifndef CONTACT_HPP
# define CONTACT_HPP

# include <cctype>
# include <iostream>
# include <cstring>
# include <iomanip> 

class Contact
{
private:
	
	std::string	name;
	std::string	nickname;
	std::string	phone_number;
	
public:

	Contact();
	bool add_contact();
	std::string get_name() const;
	std::string get_nickname() const;
	std::string get_phone_number() const;
	void print_contacts(int index) const;
	void print_contact(int index) const;
};


#endif