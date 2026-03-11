#include "PhoneBook.hpp"
#include <QFile>
#include <QTextStream>
#include <QDebug>

PhoneBook::PhoneBook(QObject *parent) : QObject(parent)
{
}

bool PhoneBook::addContact(const Contact& contact)
{
    if (!contact.isValid())
        return false;
    
    if (contacts.size() >= MAX_CAPACITY)
    {
        // Remove oldest contact (first in list)
        contacts.removeFirst();
    }
    
    contacts.append(contact);
    emit contactAdded(contact);
    return true;
}

bool PhoneBook::removeContact(int index)
{
    if (index < 0 || index >= contacts.size())
        return false;
    
    contacts.removeAt(index);
    emit contactRemoved(index);
    return true;
}

Contact PhoneBook::getContact(int index) const
{
    if (index < 0 || index >= contacts.size())
        return Contact();
    
    return contacts.at(index);
}

int PhoneBook::getContactCount() const
{
    return contacts.size();
}

QList<Contact> PhoneBook::getAllContacts() const
{
    return contacts;
}

QList<Contact> PhoneBook::searchByName(const QString& name) const
{
    QList<Contact> results;
    
    for (const Contact& contact : contacts)
    {
        if (contact.getName().contains(name, Qt::CaseInsensitive) ||
            contact.getNickname().contains(name, Qt::CaseInsensitive))
        {
            results.append(contact);
        }
    }
    
    return results;
}

QList<Contact> PhoneBook::searchByPhone(const QString& phone) const
{
    QList<Contact> results;
    
    for (const Contact& contact : contacts)
    {
        if (contact.getPhoneNumber().contains(phone))
        {
            results.append(contact);
        }
    }
    
    return results;
}

bool PhoneBook::saveToFile(const QString& filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;
    
    QTextStream out(&file);
    
    for (const Contact& contact : contacts)
    {
        out << contact.getName() << "|"
            << contact.getNickname() << "|"
            << contact.getPhoneNumber() << "|"
            << contact.getEmail() << "\n";
    }
    
    file.close();
    return true;
}

bool PhoneBook::loadFromFile(const QString& filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;
    
    contacts.clear();
    QTextStream in(&file);
    
    while (!in.atEnd())
    {
        QString line = in.readLine();
        QStringList parts = line.split("|");
        
        if (parts.size() >= 4)
        {
            Contact contact(parts[0], parts[1], parts[2], parts[3]);
            contacts.append(contact);
        }
    }
    
    file.close();
    return true;
}

void PhoneBook::clearAllContacts()
{
    contacts.clear();
    emit contactsCleared();
}
