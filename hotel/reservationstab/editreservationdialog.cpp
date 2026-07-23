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
    ui->guestNumEdit->setValidator(new QIntValidator(1, 20, this));
}

EditReservationDialog::~EditReservationDialog()
{
    //is dit wat we willen? volgens mij verwijderd dit de gehele window, niet alleen de popup
    delete ui;
}

void EditReservationDialog::setReservationData(const Reservation &res) {
    m_currentReservation = res;

    ui->nameEdit->setText(res.GuestName);
    ui->phoneNumberEdit->setText(res.GuestPhoneNumber);

    ui->addressEdit1->setText(res.address.streetLine1);
    ui->addressEdit2->setText(res.address.streetLine2);
    ui->addressEdit3->setText(res.address.city);
    ui->addressEdit4->setText(res.address.stateRegion);
    ui->addressEdit5->setText(res.address.postalCode);
    ui->addressEdit6->setText(res.address.country);

    ui->dateOfBirthEdit->setDate(res.dateOfBirth);

    ui->guestNumEdit->setText(QString::number(res.numberOfGuests));
    ui->roomEdit->setText(QString::number(res.roomNumber));

    ui->checkInEdit->setDate(res.checkIn);
    ui->checkOutEdit->setDate(res.checkOut);

}

Reservation EditReservationDialog::getReservationData() const
{
    Reservation res = m_currentReservation;

    res.GuestName = ui->nameEdit->text();
    res.GuestPhoneNumber = ui->phoneNumberEdit->text();

    res.address.streetLine1 = ui->addressEdit1->text();
    res.address.streetLine2 = ui->addressEdit2->text();
    res.address.city = ui->addressEdit3->text();
    res.address.stateRegion = ui->addressEdit4->text();
    res.address.postalCode = ui->addressEdit5->text();
    res.address.country = ui->addressEdit6->text();

    // 3. Date of Birth
    res.dateOfBirth = QDate::fromString(ui->dateOfBirthEdit->text(), "dd/MM/yyyy");

    // 4. Details
    res.numberOfGuests = ui->guestNumEdit->text().toInt();
    res.roomNumber = ui->roomEdit->text().toInt();

    // 5. Dates
    res.checkIn = ui->checkInEdit->date();
    res.checkOut = ui->checkOutEdit->date();

    return res;
}

void EditReservationDialog::on_EditReservationButtonBox_accepted()
{

    //close();

}

void EditReservationDialog::accept(){

    if(ui->checkOutEdit->date() < ui->checkInEdit->date()){
        //TODO reservatie word nog steeds aangemaakt
        QMessageBox::warning(this, "Invalid selection", "Check-out date cannot be before check-in date.");
        return;
    }
    QDialog::accept();
};

// void EditReservationDialog::closeEvent(QCloseEvent *event)
// {

//     event->ignore();
// }

