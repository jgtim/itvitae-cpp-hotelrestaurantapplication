#include "editreservationdialog.h"
#include "ui_editreservationdialog.h"

#include <QIntValidator>
#include <QMessageBox>
#include <QEvent>
#include <QCloseEvent>
#include <QDate>
#include <QComboBox>

EditReservationDialog::EditReservationDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::EditReservationDialog)
{
    ui->setupUi(this);

    connect(ui->EditReservationButtonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(ui->EditReservationButtonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);


    connect(ui->checkInEdit, &QDateEdit::dateChanged,
            this, &EditReservationDialog::updateRoomAvailability);

    connect(ui->checkOutEdit, &QDateEdit::dateChanged,
            this, &EditReservationDialog::updateRoomAvailability);

    connect(ui->guestNumEdit, &QLineEdit::textChanged,
            this, &EditReservationDialog::updateRoomAvailability);

    //TODO set maximum depending on total number of rooms in hotel. Maybe pull from database
    ui->guestNumEdit->setValidator(new QIntValidator(1, 20, this));

    ui->genderComboBox->clear();
    ui->genderComboBox->addItem("Unspecified", QVariant::fromValue(Gender::Unspecified));
    ui->genderComboBox->addItem("Male",        QVariant::fromValue(Gender::Male));
    ui->genderComboBox->addItem("Female",      QVariant::fromValue(Gender::Female));
    ui->genderComboBox->addItem("Non-Binary",  QVariant::fromValue(Gender::NonBinary));
    ui->genderComboBox->addItem("Other",       QVariant::fromValue(Gender::Other));
}

EditReservationDialog::~EditReservationDialog()
{
    //is dit wat we willen? volgens mij verwijderd dit de gehele window, niet alleen de popup
    delete ui;
}

void EditReservationDialog::setReservationData(const Reservation &res)
{
    m_currentReservation = res;

    ui->nameEdit->setText(res.guest.name);
    ui->phoneNumberEdit->setText(res.guest.phoneNumber);

    ui->addressEdit1->setText(res.guest.address.streetLine1);
    ui->addressEdit2->setText(res.guest.address.streetLine2);
    ui->addressEdit3->setText(res.guest.address.city);
    ui->addressEdit4->setText(res.guest.address.stateRegion);
    ui->addressEdit5->setText(res.guest.address.postalCode);
    ui->addressEdit6->setText(res.guest.address.country);

    ui->dateOfBirthEdit->setDate(res.guest.dateOfBirth);

    ui->guestNumEdit->setText(QString::number(res.numberOfGuests));

    ui->checkInEdit->setDate(res.checkIn);
    ui->checkOutEdit->setDate(res.checkOut);

    int genderIndex =
        ui->genderComboBox->findData(QVariant::fromValue(res.guest.gender));

    // Update the room list based on the dates.
    updateRoomAvailability();

    // Select the room of the reservation being edited.
    int roomIndex = ui->roomEdit->findData(res.roomNumber);

    if (roomIndex != -1) {
        ui->roomEdit->setCurrentIndex(roomIndex);
    }
}

void EditReservationDialog::setAvailableRooms(const QList<Room> &rooms)
{
    m_allRooms = rooms;
    populateRoomDropdown(rooms);

}

void EditReservationDialog::setExistingReservations(const QList<Reservation> &reservations)
{
    m_allReservations = reservations;
}

void EditReservationDialog::populateRoomDropdown(const QList<Room> &rooms)
{
    ui->roomEdit->clear();
    ui->roomEdit->addItem("-- Select a room --", -1);

    for (const Room &room : rooms) {
        QString roomStr = room.roomNumber;
        int roomAsInt = roomStr.remove(".").toInt();
        ui->roomEdit->addItem(room.roomNumber, roomAsInt);
    }
}
// void EditReservationDialog::setAvailableRooms(const QList<RoomRecord> &rooms)
// {
//     ui->roomEdit->clear();
//     ui->roomEdit->addItem("-- Select a room --", -1);

//     m_availableRoomNumbers.clear();

//     for (const RoomRecord &room : rooms) {
//         // "1.01" -> "101" -> 101 (int)
//         QString roomStr = room.roomNumber;
//         int roomAsInt = roomStr.remove(".").toInt();

//         // Zichtbaar in dropdown: "1.01" (mooi leesbaar)
//         // Data erachter: 101 (int, makkelijk te vergelijken)
//         ui->roomEdit->addItem(room.roomNumber, roomAsInt);

//         m_availableRoomNumbers.append(room.roomNumber);
//     }
// }

Reservation EditReservationDialog::getReservationData() const
{
    Reservation res = m_currentReservation;

    res.guest.name = ui->nameEdit->text();
    res.guest.phoneNumber = ui->phoneNumberEdit->text();

    res.guest.address.streetLine1 = ui->addressEdit1->text();
    res.guest.address.streetLine2 = ui->addressEdit2->text();
    res.guest.address.city = ui->addressEdit3->text();
    res.guest.address.stateRegion = ui->addressEdit4->text();
    res.guest.address.postalCode = ui->addressEdit5->text();
    res.guest.address.country = ui->addressEdit6->text();

    // 3. Date of Birth
    res.guest.dateOfBirth = QDate::fromString(ui->dateOfBirthEdit->text(), "dd/MM/yyyy");

    // 4. Details
    res.numberOfGuests = ui->guestNumEdit->text().toInt();
    res.roomNumber = ui->roomEdit->currentData().toInt();

    // 5. Dates
    res.checkIn = ui->checkInEdit->date();
    res.checkOut = ui->checkOutEdit->date();

    res.guest.gender = ui->genderComboBox->currentData().value<Gender>();

    return res;
}

void EditReservationDialog::on_EditReservationButtonBox_accepted()
{

    //close();

}

void EditReservationDialog::accept(){

    // Name filled in check
    if (ui->nameEdit->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Missing Information", "Please enter a guest name.");
        return;
    }

    if(ui->checkOutEdit->date() < ui->checkInEdit->date()){
        QMessageBox::warning(this, "Invalid selection", "Check-out date cannot be before check-in date.");
        return;
    }

    // Checking if reservation holder is at least 18
    QDate dob = ui->dateOfBirthEdit->date();
    QDate today = QDate::currentDate();

    if (dob.addYears(18) > today) {
        QMessageBox::warning(this, "Invalid Age", "The reservation holder must be at least 18 years old.");
        return;
    }

    QDialog::accept();
};

void EditReservationDialog::setReadOnly(bool readOnly)
{
    ui->nameEdit->setReadOnly(readOnly);
    ui->phoneNumberEdit->setReadOnly(readOnly);
    ui->dateOfBirthEdit->setReadOnly(readOnly);
    ui->genderComboBox->setEnabled(!readOnly);

    ui->addressEdit1->setReadOnly(readOnly);
    ui->addressEdit2->setReadOnly(readOnly);
    ui->addressEdit3->setReadOnly(readOnly);
    ui->addressEdit4->setReadOnly(readOnly);
    ui->addressEdit5->setReadOnly(readOnly);
    ui->addressEdit6->setReadOnly(readOnly);

    ui->guestNumEdit->setReadOnly(readOnly);
    ui->roomEdit->setEnabled(!readOnly);
    ui->checkInEdit->setReadOnly(readOnly);
    ui->checkOutEdit->setReadOnly(readOnly);

    if (readOnly) {
        this->setWindowTitle("View Reservation Details");
        ui->EditReservationButtonBox->setStandardButtons(QDialogButtonBox::Ok);
    }
}


void EditReservationDialog::updateRoomAvailability()
{
    QDate checkIn = ui->checkInEdit->date();
    QDate checkOut = ui->checkOutEdit->date();

    int previouslySelected = ui->roomEdit->currentData().toInt();
    if (previouslySelected == -1 && !m_currentReservation.ReservationID.isEmpty()) {
        previouslySelected = m_currentReservation.roomNumber;
    }
    QList<Room> availableRooms;

    for (const Room &room : m_allRooms) {
        QString roomStr = room.roomNumber;
        int roomAsInt = roomStr.remove(".").toInt();

        bool isTaken = false;

        for (const Reservation &res : m_allReservations) {

            // Sla de reservering over die we nu zelf aan het bewerken zijn
            if (!m_currentReservation.ReservationID.isEmpty() &&
                res.ReservationID == m_currentReservation.ReservationID) {
                continue;
            }

            if (res.roomNumber == roomAsInt) {

                // Controleer of de reserveringen elkaar overlappen
                if (checkIn < res.checkOut && res.checkIn < checkOut) {
                    isTaken = true;
                    break;
                }
            }
        }

        if (!isTaken && room.adults >= ui->guestNumEdit->text().toInt()) {
            availableRooms.append(room);
        }
    }

    populateRoomDropdown(availableRooms);

    // Probeer de eerder geselecteerde kamer opnieuw te selecteren
    int idx = ui->roomEdit->findData(previouslySelected);

    if (idx != -1) {
        ui->roomEdit->setCurrentIndex(idx);
    }
}
// void EditReservationDialog::closeEvent(QCloseEvent *event)
// {

//     event->ignore();
// }

