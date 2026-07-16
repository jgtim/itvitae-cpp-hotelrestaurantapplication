#include "reservationstab.h"
#include "ui_reservationstab.h"
#include "editreservationdialog.h"

#include <QMessageBox>
#include <iostream>
#include <QDate>
#include <QDebug>

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

    proxyModel = new QSortFilterProxyModel(this);
    proxyModel->setSourceModel(model);
    proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    proxyModel->setFilterKeyColumn(-1);

    ui->ReservationsTabReservationsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->ReservationsTabReservationsTable->setModel(proxyModel);
    ui->ReservationsTabReservationsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    connect(ui->ReservationsTabSearchfield, &QLineEdit::textChanged,
            proxyModel, &QSortFilterProxyModel::setFilterFixedString);

    connect(ui->ReservationTabEditButton, &QPushButton::clicked, this, &ReservationsTab::on_ReservationTabEditButton_clicked);
    connect(ui->ReservationTabCancelButton, &QPushButton::clicked, this, &ReservationsTab::on_ReservationTabCancelButton_clicked);

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
    QModelIndex proxyIndex = ui->ReservationsTabReservationsTable->currentIndex();

    if (!proxyIndex.isValid()) {
        QMessageBox::warning(this, "No Selection", "Please click on a reservation first to edit it.");
        return;
    }

    QModelIndex sourceIndex = proxyModel->mapToSource(proxyIndex);
    int selectedRow = sourceIndex.row();

    QString currentName = model->item(selectedRow, 1)->text();
    QString currentRoom = model->item(selectedRow, 2)->text();
    QDate currentCheckIn = QDate::fromString(model->item(selectedRow, 3)->text(), "dd/MM/yyyy");
    QDate currentCheckOut = QDate::fromString(model->item(selectedRow, 4)->text(), "dd/MM/yyyy");

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

}

void ReservationsTab::on_ReservationTabCancelButton_clicked()
{
    QModelIndex proxyIndex = ui->ReservationsTabReservationsTable->currentIndex();

    if (!proxyIndex.isValid()) {
        QMessageBox::warning(this, "No Selection", "Please click on a reservation first to cancel it.");
        return;
    }

    QModelIndex sourceIndex = proxyModel->mapToSource(proxyIndex);
    int selectedRow = sourceIndex.row();

    QString guestName = model->item(selectedRow, 1)->text();

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this,
                                  "Confirm Cancellation",
                                  QString("Are you sure you want to cancel the reservation for %1?").arg(guestName),
                                  QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        model->removeRow(selectedRow);

        emit dataChangedNotification();
    }
}

// Placeholder method for adding mock data - REMOVE LATER
void ReservationsTab::addMockData()
{
    QList<QStandardItem*> row1;
    row1 << new QStandardItem("0001")
         << new QStandardItem("UutWijk")
         << new QStandardItem("101")
         << new QStandardItem("12/07/2026")
         << new QStandardItem("15/07/2026");
    model->appendRow(row1);

    QList<QStandardItem*> row2;
    row2 << new QStandardItem("0002")
         << new QStandardItem("Zuensai")
         << new QStandardItem("102")
         << new QStandardItem("14/07/2026")
         << new QStandardItem("20/07/2026");
    model->appendRow(row2);

    QList<QStandardItem*> row3;
    row3 << new QStandardItem("0003")
         << new QStandardItem("DoubleVegetaMickeyMouse")
         << new QStandardItem("103")
         << new QStandardItem("21/08/2026")
         << new QStandardItem("25/08/2026");
    model->appendRow(row3);

    QList<QStandardItem*> row4;
    row4 << new QStandardItem("0004")
         << new QStandardItem("jochemvdsteen")
         << new QStandardItem("104")
         << new QStandardItem("10/06/2026")
         << new QStandardItem("20/06/2026");
    model->appendRow(row4);

    QList<QStandardItem*> row5;
    row5 << new QStandardItem("0005")
         << new QStandardItem("IanSchaafstra")
         << new QStandardItem("105")
         << new QStandardItem("08/08/2026")
         << new QStandardItem("18/08/2026");
    model->appendRow(row5);
}
void ReservationsTab::on_ReservationsTabNewReservationButton_clicked()
{

    EditReservationDialog dialog(this);

    if (dialog.exec() == QDialog::Accepted) {

        //TODO set new ID; get last number from existing database and add 1
        QString newID = QString::number(6);
        QString newName = dialog.getGuestName();
        QString newRoom = dialog.getRoomNumber();
        QString newCheckIn = dialog.getCheckInDate();
        QString newCheckOut = dialog.getCheckOutDate();

        QList<QStandardItem*> newRow;
        newRow << new QStandardItem(newID)
             << new QStandardItem(newName)
             << new QStandardItem(newRoom)
             << new QStandardItem(newCheckIn)
             << new QStandardItem(newCheckOut);
        model->appendRow(newRow);

    }
}

