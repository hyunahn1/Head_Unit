#ifndef ADDCONTACT_HPP
#define ADDCONTACT_HPP

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>

class AddContact : public QDialog
{
    Q_OBJECT

public:
    AddContact(QWidget *parent = nullptr);

    QString getName() const;
    QString getPhone() const;
    QString getEmail() const;

private:
    QLineEdit* nameEdit;
    QLineEdit* mailEdit;
    QLineEdit* phoneEdit;
    QPushButton* okButton;
    QPushButton* cancelButton;
};

#endif