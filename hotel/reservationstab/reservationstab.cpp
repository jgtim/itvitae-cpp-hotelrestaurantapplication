#include "reservationstab.h"
#include "ui_reservationstab.h"
#include "editreservationdialog.h"

#include <QMessageBox>
#include <iostream>

ReservationsTab::ReservationsTab(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ReservationsTab)
{
    ui->setupUi(this);

    model = new QStandardItemModel(0, 5, this);

    model->setHeaderData(0, Qt::Horizontal, "Reservation ID");
    model->setHeaderData(1, Qt::Horizontal, "Guest Name");
    model->setHeaderData(2, Qt::Horizontal, "Room Number");
    model->setHeaderData(3, Qt::Horizontal, "Check-In Date");
    model->setHeaderData(4, Qt::Horizontal, "Check-Out Date");

    ui->ReservationsTabReservationsTable->setModel(model);
    ui->ReservationsTabReservationsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    connect(ui->ReservationTabEditButton, &QPushButton::clicked, this, &ReservationsTab::on_ReservationTabEditButton_clicked);

    // For now - REMOVE LATER:
    addMockData();

}

ReservationsTab::~ReservationsTab()
{
    delete ui;
}

void ReservationsTab::onDataChanged()
{
    qDebug() << "Reservations Tab received notification that data changed. Refreshing UI...";
}

void ReservationsTab::on_ReservationTabEditButton_clicked()
{
    QModelIndex currentIndex = ui->ReservationsTabReservationsTable->currentIndex();

    if (!currentIndex.isValid()) {
        QMessageBox::warning(this, "No Selection", "Please click on a reservation first to edit it.");
        return;
    }

    int selectedRow = currentIndex.row();

    QString currentName = model->item(selectedRow, 1)->text();
    QString currentRoom = model->item(selectedRow, 2)->text();
    QString currentCheckIn = model->item(selectedRow, 3)->text();
    QString currentCheckOut = model->item(selectedRow, 4)->text();

    EditReservationDialog dialog(this);
    dialog.setReservationData(currentName, currentRoom, currentCheckIn, currentCheckOut);

    if (dialog.exec() == QDialog::Accepted) {

        QString updatedName = dialog.getGuestName();
        QString updatedRoom = dialog.getRoomNumber();
        QString updatedCheckIn = dialog.getCheckInDate();
        QString updatedCheckOut = dialog.getCheckOutDate();


        model->item(selectedRow, 1)->setText(updatedName);
        model->item(selectedRow, 2)->setText(updatedRoom);
        model->item(selectedRow, 3)->setText(updatedCheckIn);
        model->item(selectedRow, 4)->setText(updatedCheckOut);
    }
    std:: cout << "TOO FAR" << std::endl;

}

// Placeholder method for adding mock data - REMOVE LATER
void ReservationsTab::addMockData()
{
    QList<QStandardItem*> row1;
    row1 << new QStandardItem("0001")
         << new QStandardItem("UutWijk")
         << new QStandardItem("101")
         << new QStandardItem("12-07-2026")
         << new QStandardItem("15-07-2026");
    model->appendRow(row1);

    QList<QStandardItem*> row2;
    row2 << new QStandardItem("0002")
         << new QStandardItem("Zuensai")
         << new QStandardItem("102")
         << new QStandardItem("14-07-2026")
         << new QStandardItem("20-07-2026");
    model->appendRow(row2);

    QList<QStandardItem*> row3;
    row3 << new QStandardItem("0003")
         << new QStandardItem("DoubleVegetaMickeyMouse")
         << new QStandardItem("103")
         << new QStandardItem("21-08-2026")
         << new QStandardItem("25-08-2026");
    model->appendRow(row3);

    QList<QStandardItem*> row4;
    row4 << new QStandardItem("0004")
         << new QStandardItem("jochemvdsteen")
         << new QStandardItem("104")
         << new QStandardItem("10-06-2026")
         << new QStandardItem("20-06-2026");
    model->appendRow(row4);

    QList<QStandardItem*> row5;
    row5 << new QStandardItem("0005")
         << new QStandardItem("IanSchaafstra")
         << new QStandardItem("105")
         << new QStandardItem("08-08-2026")
         << new QStandardItem("18-08-2026");
    model->appendRow(row5);
}
void ReservationsTab::on_ReservationsTabNewReservationButton_clicked()
{
    std::cout << "CLICKITY CLICK" << std::endl;

    EditReservationDialog dialog(this);

    if (dialog.exec() == QDialog::Accepted) {

        QString newName = dialog.getGuestName();
        QString newRoom = dialog.getRoomNumber();
        QString newCheckIn = dialog.getCheckInDate();
        QString newCheckOut = dialog.getCheckOutDate();

        QList<QStandardItem*> newRow;
        newRow << new QStandardItem("0006")
             << new QStandardItem(newName)
             << new QStandardItem(newRoom)
             << new QStandardItem(newCheckIn)
             << new QStandardItem(newCheckOut);
        model->appendRow(newRow);

    }
}


