#include "editreservationdialog.h"
#include "ui_editreservationdialog.h"

#include <iostream>
#include <QIntValidator>
#include <QMessageBox>
#include <QEvent>
#include <QCloseEvent>
#include <QDate>

EditReservationDialog::EditReservationDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::EditReservationDialog)
{
    ui->setupUi(this);

    connect(ui->EditReservationButtonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(ui->EditReservationButtonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    //TODO set maximum depending on total number of rooms in hotel. Maybe pull from database
    ui->roomEdit->setValidator( new QIntValidator(0, 999, this) );
}

EditReservationDialog::~EditReservationDialog()
{
    //is dit wat we willen? volgens mij verwijderd dit de gehele window, niet alleen de popup
    delete ui;
}

void EditReservationDialog::setReservationData(const QString &name, const QString &room, const QDate &checkIn, const QDate &checkOut) {
    ui->nameEdit->setText(name);
    ui->roomEdit->setText(room);
    ui->checkInEdit->setDate(checkIn);
    ui->checkOutEdit->setDate(checkOut);
}

QString EditReservationDialog::getGuestName() const { return ui->nameEdit->text(); }
QString EditReservationDialog::getRoomNumber() const { return ui->roomEdit->text(); }
QString EditReservationDialog::getCheckInDate() const { return ui->checkInEdit->date().toString("dd/MM/yyyy"); }
QString EditReservationDialog::getCheckOutDate() const { return ui->checkInEdit->date().toString("dd/MM/yyyy"); }

void EditReservationDialog::on_EditReservationButtonBox_accepted()
{
    // if(ui->checkOutEdit->date() < ui->checkInEdit->date()){
    //     //TODO reservatie word nog steeds aangemaakt
    //     event->ignore();
    //     QMessageBox::warning(this, "Invalid selection", "checkout date cannot be before checkin date.");
    //     return;
    // }
    close();

}

// void EditReservationDialog::closeEvent(QCloseEvent *event)
// {

//     event->ignore();
// }

