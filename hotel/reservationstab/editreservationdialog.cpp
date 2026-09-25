#include "hotel/reservationstab/editreservationdialog.h"
#include "hotel/reservationstab/ui_editreservationdialog.h"
#include "hotel/reservationstab/Guest/IDType.h"
#include "hotel/reservationstab/Guest/PaymentMethod.h"

#include <QIntValidator>
#include <QMessageBox>
#include <QDate>
#include <QComboBox>

EditReservationDialog::EditReservationDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::EditReservationDialog)
{
    ui->setupUi(this);

    // This makes a calendar pop up when selecting dates
    ui->checkInEdit->setCalendarPopup(true);
    ui->checkOutEdit->setCalendarPopup(true);
    ui->dateOfBirthEdit->setCalendarPopup(true);

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

    // Setting minimum check-in date to today
    ui->checkInEdit->setMinimumDate(QDate::currentDate());
    ui->checkOutEdit->setMinimumDate(QDate::currentDate().addDays(1));

    // Checking contact info text fields for maximum number of characters, while user is typing
    setupFieldLimit(ui->nameEdit, 50, "Name");
    setupFieldLimit(ui->emailEdit, 50, "Email");
    setupFieldLimit(ui->phoneNumberEdit, 15, "Phone number");

    // Checking address info fields for max length
    setupFieldLimit(ui->addressStreetNumberEdit, 100, "Street and number");
    setupFieldLimit(ui->addressOptionalEdit, 100, "Optional house information");
    setupFieldLimit(ui->addressCityEdit, 50, "City");
    setupFieldLimit(ui->addressStateRegionEdit, 50, "State, region or province");
    setupFieldLimit(ui->addressPostalEdit, 12, "Postal code");
    setupFieldLimit(ui->addressCountryEdit, 50, "Country");

    ui->genderComboBox->clear();
    ui->genderComboBox->addItem("Unspecified", QVariant::fromValue(Gender::Unspecified));
    ui->genderComboBox->addItem("Male",        QVariant::fromValue(Gender::Male));
    ui->genderComboBox->addItem("Female",      QVariant::fromValue(Gender::Female));
    ui->genderComboBox->addItem("Non-Binary",  QVariant::fromValue(Gender::NonBinary));
    ui->genderComboBox->addItem("Other",       QVariant::fromValue(Gender::Other));

    ui->IDTypeComboBox->clear();
    ui->IDTypeComboBox->addItem("Passport",         QVariant::fromValue(IDType::Passport));
    ui->IDTypeComboBox->addItem("Driver's License", QVariant::fromValue(IDType::DriverLicense));
    ui->IDTypeComboBox->addItem("National ID",      QVariant::fromValue(IDType::NationalID));
    ui->IDTypeComboBox->addItem("Other",            QVariant::fromValue(IDType::Other));

    ui->paymentMethodComboBox->clear();
    ui->paymentMethodComboBox->addItem("Credit Card",   QVariant::fromValue(PaymentMethod::CreditCard));
    ui->paymentMethodComboBox->addItem("Debit Card",    QVariant::fromValue(PaymentMethod::DebitCard));
    ui->paymentMethodComboBox->addItem("Cash",          QVariant::fromValue(PaymentMethod::Cash));
    ui->paymentMethodComboBox->addItem("Bank Transfer", QVariant::fromValue(PaymentMethod::BankTransfer));
    ui->paymentMethodComboBox->addItem("PayPal",        QVariant::fromValue(PaymentMethod::PayPal));
    ui->paymentMethodComboBox->addItem("Other",         QVariant::fromValue(PaymentMethod::Other));

    this->setWindowTitle("New Reservation");
}

EditReservationDialog::~EditReservationDialog()
{
    delete ui;
}

void EditReservationDialog::setReservationData(const Reservation &res)
{
    this->setWindowTitle("Edit Reservation");
    m_currentReservation = res;

    ui->nameEdit->setText(res.guest.name);
    ui->phoneNumberEdit->setText(res.guest.phoneNumber);
    ui->emailEdit->setText(res.guest.email);

    ui->addressStreetNumberEdit->setText(res.guest.address.streetLine1);
    ui->addressOptionalEdit->setText(res.guest.address.streetLine2);
    ui->addressCityEdit->setText(res.guest.address.city);
    ui->addressStateRegionEdit->setText(res.guest.address.stateRegion);
    ui->addressPostalEdit->setText(res.guest.address.postalCode);
    ui->addressCountryEdit->setText(res.guest.address.country);

    ui->dateOfBirthEdit->setDate(res.guest.dateOfBirth);

    ui->guestNumEdit->setText(QString::number(res.numberOfGuests));

    ui->checkInEdit->setDate(res.checkIn);
    ui->checkOutEdit->setDate(res.checkOut);

    // Set active index for Gender
    int genderIndex = ui->genderComboBox->findData(QVariant::fromValue(res.guest.gender));
    if (genderIndex != -1) ui->genderComboBox->setCurrentIndex(genderIndex);

    // Set active index for ID Type
    int idTypeIndex = ui->IDTypeComboBox->findData(QVariant::fromValue(res.guest.idType));
    if (idTypeIndex != -1) ui->IDTypeComboBox->setCurrentIndex(idTypeIndex);

    // Set active index for Payment Method
    int paymentMethodIndex = ui->paymentMethodComboBox->findData(QVariant::fromValue(res.guest.paymentMethod));
    if (paymentMethodIndex != -1) ui->paymentMethodComboBox->setCurrentIndex(paymentMethodIndex);

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
    ui->roomEdit->addItem("-- Select a room --", QString(""));

    for (const Room &room : rooms) {
        ui->roomEdit->addItem(room.roomNumber, room.roomNumber);
    }
}


Reservation EditReservationDialog::getReservationData() const
{
    Reservation res = m_currentReservation;

    // Guest contact
    res.guest.name = ui->nameEdit->text();
    res.guest.phoneNumber = ui->phoneNumberEdit->text();
    res.guest.email = ui->emailEdit->text();

    // Address
    res.guest.address.streetLine1 = ui->addressStreetNumberEdit->text();
    res.guest.address.streetLine2 = ui->addressOptionalEdit->text();
    res.guest.address.city = ui->addressCityEdit->text();
    res.guest.address.stateRegion = ui->addressStateRegionEdit->text();
    res.guest.address.postalCode = ui->addressPostalEdit->text();
    res.guest.address.country = ui->addressCountryEdit->text();

    // Date of Birth
    res.guest.dateOfBirth = ui->dateOfBirthEdit->date();

    // Details
    res.numberOfGuests = ui->guestNumEdit->text().toInt();
    res.roomNumber = ui->roomEdit->currentData().toString();

    // Dates
    res.checkIn = ui->checkInEdit->date();
    res.checkOut = ui->checkOutEdit->date();

    // Enums
    res.guest.gender        = ui->genderComboBox->currentData().value<Gender>();
    res.guest.idType        = ui->IDTypeComboBox->currentData().value<IDType>();
    res.guest.paymentMethod = ui->paymentMethodComboBox->currentData().value<PaymentMethod>();

    return res;
}


void EditReservationDialog::accept() {
    // Name
    if (ui->nameEdit->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Missing Information", "Please enter a name.");
        ui->nameEdit->setFocus();
        return;
    }

    // Phone Number
    if (ui->phoneNumberEdit->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Missing Information", "Please enter a phone number.");
        ui->phoneNumberEdit->setFocus();
        return;
    }

    // Email
    if (ui->emailEdit->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Missing Information", "Please enter a valid email address.");
        ui->emailEdit->setFocus();
        return;
    }

    static const QRegularExpression emailRegex(R"(^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$)");
    if (!ui->emailEdit->text().trimmed().isEmpty() &&
        !emailRegex.match(ui->emailEdit->text().trimmed()).hasMatch()) {
        QMessageBox::warning(this, "Invalid Email", "Please enter a valid email address.");
        ui->emailEdit->setFocus();
        return;
    }

    // Address
    bool missingStreet = ui->addressStreetNumberEdit->text().trimmed().isEmpty();
    bool missingCity   = ui->addressCityEdit->text().trimmed().isEmpty();
    bool missingPostal = ui->addressPostalEdit->text().trimmed().isEmpty();
    bool missingCountry= ui->addressCountryEdit->text().trimmed().isEmpty();

    if (missingStreet || missingCity || missingPostal || missingCountry) {
        QMessageBox::warning(
            this,
            "Incomplete Address", "Please fill in our required address information:\n"
            "• Street and number\n"
            "• City\n"
            "• Postal code\n"
            "• Country"
            );
        if (missingStreet) ui->addressStreetNumberEdit->setFocus();
        else if (missingCity) ui->addressCityEdit->setFocus();
        else if (missingPostal) ui->addressPostalEdit->setFocus();
        else if (missingCountry) ui->addressCountryEdit->setFocus();
        return;
    }

    // Room number
    if (ui->roomEdit->currentData().toString().isEmpty()) {
        QMessageBox::warning(this, "Missing Information", "Please select an available room.");
        ui->roomEdit->setFocus();
        return;
    }

    // Check-out before check-in
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
    ui->emailEdit->setReadOnly(readOnly);
    ui->dateOfBirthEdit->setReadOnly(readOnly);
    ui->genderComboBox->setEnabled(!readOnly);
    ui->IDTypeComboBox->setEnabled(!readOnly);
    ui->paymentMethodComboBox->setEnabled(!readOnly);

    ui->addressStreetNumberEdit->setReadOnly(readOnly);
    ui->addressOptionalEdit->setReadOnly(readOnly);
    ui->addressCityEdit->setReadOnly(readOnly);
    ui->addressStateRegionEdit->setReadOnly(readOnly);
    ui->addressPostalEdit->setReadOnly(readOnly);
    ui->addressCountryEdit->setReadOnly(readOnly);

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

    QString previouslySelected = ui->roomEdit->currentData().toString();

    if (previouslySelected.isEmpty() && !m_currentReservation.ReservationID.isEmpty()) {
        previouslySelected = m_currentReservation.roomNumber;
    }

    QList<Room> availableRooms;

    for (const Room &room : m_allRooms) {
        bool isTaken = false;

        for (const Reservation &res : m_allReservations) {
            // Skip checking against the reservation currently being edited
            if (!m_currentReservation.ReservationID.isEmpty() &&
                res.ReservationID == m_currentReservation.ReservationID) {
                continue;
            }

            // Direct QString comparison:
            if (res.roomNumber == room.roomNumber) {
                // Check if dates overlap
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


//
void EditReservationDialog::setupFieldLimit(QLineEdit *lineEdit, int maxLength, const QString &fieldName)
{
    lineEdit->setMaxLength(maxLength);
    lineEdit->setProperty("fieldName", fieldName); // Attach field name the user is typing in to warning message
    connect(lineEdit, &QLineEdit::textEdited, this, &EditReservationDialog::checkFieldLength);
}


void EditReservationDialog::checkFieldLength(const QString &text)
{
    auto *edit = qobject_cast<QLineEdit*>(sender());
    if (edit && text.length() >= edit->maxLength()) {
        QString name = edit->property("fieldName").toString();
        QMessageBox::information(this, "Limit Reached",
                                 QString("%1 cannot exceed %2 characters.").arg(name).arg(edit->maxLength()));
    }
}