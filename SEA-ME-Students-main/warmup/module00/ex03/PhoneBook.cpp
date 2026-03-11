#include "PhoneBook.hpp"

bool	PhoneBook::add_contact()
{
	if (this->nb_contacts < this->CAPACITY)
	{
		Contact* new_contact = new Contact();
		if (new_contact->add_contact())
		{
			if (this->contacts[this->nb_contacts])
				delete (this->contacts[this->nb_contacts]);
			this->contacts[this->nb_contacts] = new_contact;
			this->nb_contacts++;
			return (true);
		}
		delete (new_contact);
	}
	else
	{
		Contact* new_contact = new Contact();
		if (new_contact->add_contact())
		{
			this->nb_contacts = 0;
			delete (this->contacts[this->nb_contacts]);
			this->contacts[this->nb_contacts] = new_contact;
			this->nb_contacts++;
			return (true);
		}
		delete (new_contact);
	}
	return (true);
}

void PhoneBook::print_contact(int index)
{
	Contact* contact = contacts[index];
	
	contact->print_contact(index);
}

PhoneBook::~PhoneBook()
{
	for (int i = 0; i < 8; ++i)
	{
		if (this->contacts[i])
			delete (this->contacts[i]);
	}
}

PhoneBook::PhoneBook() : nb_contacts(0)
{
	for (int i = 0; i < 8; i++)
		contacts[i] = NULL;
}

PhoneBook::PhoneBook(const Contact &contact) : contact (contact), nb_contacts(0)
{
	for (int i = 0; i < 8; i++)
		contacts[i] = NULL;
}

int PhoneBook::get_nb_contacts() const 
{
	return nb_contacts;
}

void PhoneBook::print_contacts()
{
	for (int i = 0; i < 8; i++)
	{
		if (contacts[i])
		{	
			Contact* contact = contacts[i];
			contact->print_contacts(i);
		}
		else
			return ;
	}
}

void PhoneBook::remove_contact(int i)
{
    if (i < 8 && this->contacts[i])
    {
       delete (this->contacts[i]);
       this->contacts[i] = NULL;
    }
}