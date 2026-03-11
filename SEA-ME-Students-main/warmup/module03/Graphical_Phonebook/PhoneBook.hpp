#ifndef PHONEBOOK_HPP
#define PHONEBOOK_HPP

#include <QObject>
#include <QList>
#include <QString>
#include "Contact.hpp"

class PhoneBook : public QObject
{
    Q_OBJECT

private:
    QList<Contact> contacts;
    static const int MAX_CAPACITY = 8;

public:
    explicit PhoneBook(QObject *parent = nullptr);
    
    bool addContact(const Contact& contact);
    bool removeContact(int index);
    Contact getContact(int index) const;
    int getContactCount() const;
    QList<Contact> getAllContacts() const;
    
    // Search functionality
    QList<Contact> searchByName(const QString& name) const;
    QList<Contact> searchByPhone(const QString& phone) const;
    
    // File I/O
    bool saveToFile(const QString& filename);
    bool loadFromFile(const QString& filename);

signals:
    void contactAdded(const Contact& contact);
    void contactRemoved(int index);
    void contactsCleared();
    void contactUpdated(int index);

public slots:
    void clearAllContacts();
};

#endif
