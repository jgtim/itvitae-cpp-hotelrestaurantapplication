#include "editreservationdialog.h"
#include "ui_editreservationdialog.h"

EditReservationDialog::EditReservationDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::EditReservationDialog)
{
    ui->setupUi(this);

    connect(ui->EditReservationButtonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(ui->EditReservationButtonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

EditReservationDialog::~EditReservationDialog()
{
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