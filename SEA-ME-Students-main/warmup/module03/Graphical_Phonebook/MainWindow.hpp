#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>
#include <QPushButton>
#include <QLineEdit>
#include <QTableWidget>
#include "Contact.hpp"
#include <QCloseEvent>
#include <QFile>
#include <QTextStream>

class MainWindow : public QMainWindow
{
    Q_OBJECT

    private:

        QLineEdit* nameEdit;
        QLineEdit* searchEdit;
        QPushButton* addButton;
        QPushButton* removeButton;
        QPushButton* searchButton;
        QTableWidget* contactTable;
        
        void saveContacts(const QString& filename);
        void loadContacts(const QString& filename);

    private slots:
        void onSearchContact();
        void onAddContact();
        void onRemoveContact();
    public:
        MainWindow(QWidget *parent = nullptr);

    protected:
        void closeEvent(QCloseEvent* event) override;
};
#endif