#include "MainWindow.hpp"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    resize(400, 300);

    searchEdit = new QLineEdit(this);
    searchEdit->move(10, 10);
    searchEdit->resize(200, 25);

    searchButton = new QPushButton("Search", this);
    searchButton->move(210, 10);
    searchButton->resize(80, 25);
    connect(searchButton, &QPushButton::clicked, this, &MainWindow::onSearchContact);

    addButton = new QPushButton("Add", this);
    addButton->move(100, 210);
    connect(addButton, &QPushButton::clicked, this, &MainWindow::onAddContact);
    removeButton = new QPushButton("Remove", this);
    removeButton->move(10, 210);
    connect(removeButton, &QPushButton::clicked, this, &MainWindow::onRemoveContact);
    contactTable = new QTableWidget(0, 3, this);
    contactTable->resize(300, 150);
    contactTable->move(10, 50);
    contactTable->setHorizontalHeaderItem(0, new QTableWidgetItem("Name"));
    contactTable->setHorizontalHeaderItem(1, new QTableWidgetItem("Phonenumber"));
    contactTable->setHorizontalHeaderItem(2, new QTableWidgetItem("email"));
    loadContacts("contacts.csv");

}

void MainWindow::onAddContact()
{
    AddContact dlg(this);
    if (dlg.exec() == QDialog::Accepted) {
        int row = contactTable->rowCount();
        contactTable->insertRow(row);
        contactTable->setItem(row, 0, new QTableWidgetItem(dlg.getName()));
        contactTable->setItem(row, 1, new QTableWidgetItem(dlg.getPhone()));
        contactTable->setItem(row, 2, new QTableWidgetItem(dlg.getEmail()));
    }
}


void MainWindow::onRemoveContact()
{
    auto selectedItems = contactTable->selectedItems();
    if (!selectedItems.isEmpty())
    {
        int row = selectedItems.first()->row();
        contactTable->removeRow(row);
    }
}

void MainWindow::onSearchContact()
{
    QString text = searchEdit->text();
    contactTable->clearSelection();

    for (int i = 0; i < contactTable->rowCount(); i++)
    {
        for (int col = 0; col < contactTable->columnCount(); ++col)
        {
            auto item = contactTable->item(i, col);
            if (item && item->text().contains(text, Qt::CaseInsensitive))
            {
                item->setSelected(true);
                break;
            }
        }
    }
}

void MainWindow::saveContacts(const QString& filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;

    QTextStream out(&file);

    for (int i = 0; i < contactTable->rowCount(); i++)
    {
        QString name = contactTable->item(i, 0)->text();
        QString phone = contactTable->item(i, 1)->text();
        QString email = contactTable->item(i, 2)->text();
        out << name << "," << phone << "," << email << "\n";
    }
    file.close();
}


void MainWindow::loadContacts(const QString& filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    contactTable->setRowCount(0);
    QTextStream in(&file);
    while(!in.atEnd())
    {
        QString line = in.readLine();
        QStringList parts = line.split(",");

        if (parts.size() == 3)
        {
            int row = contactTable->rowCount();
            contactTable->insertRow(row);
            contactTable->setItem(row, 0, new QTableWidgetItem(parts[0]));
            contactTable->setItem(row, 1, new QTableWidgetItem(parts[1]));
            contactTable->setItem(row, 2, new QTableWidgetItem(parts[2]));
        }
    }
    file.close();
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    saveContacts("contacts.csv");
    QMainWindow::closeEvent(event);
}