#include "Contact.hpp"

AddContact::AddContact(QWidget *parent) : QDialog(parent)
{
    setWindowTitle("Add Contact");
    setModal(true);
    resize(250, 300);
    nameEdit = new QLineEdit(this);
    nameEdit->setPlaceholderText("Name");
    nameEdit->move(10, 10);
    phoneEdit = new QLineEdit(this);
    phoneEdit->setPlaceholderText("Phonenumber");
    phoneEdit->move(10, 50);
    mailEdit = new QLineEdit(this);
    mailEdit->setPlaceholderText("Email");
    mailEdit->move(10, 90);
    okButton = new QPushButton("OK", this);
    okButton->move(10, 250);
    cancelButton = new QPushButton("Cancel", this);
    cancelButton->move(100, 250);
    connect(okButton, &QPushButton::clicked, this, &QDialog::accept);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);

    auto layout = new QVBoxLayout();
    layout->addWidget(nameEdit);
    layout->addWidget(phoneEdit);
    layout->addWidget(mailEdit);

    auto buttonlayout = new QVBoxLayout();
    buttonlayout->addWidget(okButton);
    buttonlayout->addWidget(cancelButton);

    layout->addLayout(buttonlayout);
}

QString AddContact::getName() const 
{
    return nameEdit->text();
}

QString AddContact::getPhone() const 
{
    return phoneEdit->text();
}

QString AddContact::getEmail() const 
{
    return mailEdit->text();
}