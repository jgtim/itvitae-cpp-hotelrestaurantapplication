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

void EditReservationDialog::setReservationData(const Reservation &res) {
    m_currentReservation = res;

    // Populate UI inputs from struct fields
    ui->nameEdit->setText(res.GuestName);
    // ui->phoneLineEdit->setText(res.GuestPhoneNumber);
    ui->checkInEdit->setDate(res.checkIn);
    ui->checkOutEdit->setDate(res.checkOut);

    // ui->guestsSpinBox->setValue(res.numberOfGuests);
    // ui->roomNumberEdit->setValue(res.roomNumber);

    // ui->streetLineEdit->setText(res.address.streetLine1);
    // ui->cityLineEdit->setText(res.address.city);
    // ui->postalCodeLineEdit->setText(res.address.postalCode);
    // ui->countryLineEdit->setText(res.address.country);

    // ui->genderComboBox->setCurrentText(genderToString(res.gender));
}

Reservation EditReservationDialog::getReservationData() const
{
    Reservation res = m_currentReservation;

    // Pull modified values from UI widgets
    res.GuestName = ui->nameEdit->text();
    // res.GuestPhoneNumber = ui->phoneLineEdit->text();
    res.checkIn = ui->checkInEdit->date();
    res.checkOut = ui->checkOutEdit->date();

    // res.numberOfGuests = ui->guestsSpinBox->value();
    // res.roomNumber = ui->roomNumberEdit->value();

    // res.address.streetLine1 = ui->streetLineEdit->text();
    // res.address.city = ui->cityLineEdit->text();
    // res.address.postalCode = ui->postalCodeLineEdit->text();
    // res.address.country = ui->countryLineEdit->text();

    // res.gender = genderFromString(ui->genderComboBox->currentText());

    return res;
}

void EditReservationDialog::on_EditReservationButtonBox_accepted()
{

    //close();

}

void EditReservationDialog::accept(){

    if(ui->checkOutEdit->date() < ui->checkInEdit->date()){
        //TODO reservatie word nog steeds aangemaakt
        QMessageBox::warning(this, "Invalid selection", "checkout date cannot be before checkin date.");
        return;
    }else{
        QDialog::accept();
    }
};

// void EditReservationDialog::closeEvent(QCloseEvent *event)
// {

//     event->ignore();
// }

