#include "editreservationdialog.h"
#include "ui_editreservationdialog.h"

#include <iostream>
#include <QIntValidator>

EditReservationDialog::EditReservationDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::EditReservationDialog)
{
    ui->setupUi(this);

    connect(ui->EditReservationButtonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(ui->EditReservationButtonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    //TODO set maximum depending on total number of rooms in hotel. Maybe pull from database
    ui->roomEdit->setValidator( new QIntValidator(2, 999, this) );
}

EditReservationDialog::~EditReservationDialog()
{
    //is dit wat we willen? volgens mij verwijderd dit de gehele window, niet alleen de popup
    delete ui;
}

void EditReservationDialog::setReservationData(const QString &name, const QString &room, const QString &checkIn, const QString &checkOut) {
    ui->nameEdit->setText(name);
    ui->roomEdit->setText(room);
    ui->checkInEdit->setText(checkIn);
    ui->checkOutEdit->setText(checkOut);
}

QString EditReservationDialog::getGuestName() const { return ui->nameEdit->text(); }
QString EditReservationDialog::getRoomNumber() const { return ui->roomEdit->text(); }
QString EditReservationDialog::getCheckInDate() const { return ui->checkInEdit->text(); }
QString EditReservationDialog::getCheckOutDate() const { return ui->checkOutEdit->text(); }

void EditReservationDialog::on_EditReservationButtonBox_accepted()
{
    //room number has to be an int. eventually we want to select among available rooms
    //QString newCheckIn = dialog.getCheckInDate();
    //QString newCheckOut = dialog.getCheckOutDate();
    close();

}

