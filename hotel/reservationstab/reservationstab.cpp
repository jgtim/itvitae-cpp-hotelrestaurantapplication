#include "hotel/reservationstab/reservationstab.h"
#include "hotel/reservationstab/ui_reservationstab.h"
#include "hotel/reservationstab/editreservationdialog.h"
#include "readers/reservationserializer.h"

#include <QMessageBox>
#include <QDate>
#include <QDebug>
#include <QString>
#include <QSqlError>
#include <QSqlRecord>
#include <QJsonObject>

ReservationsTab::ReservationsTab(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ReservationsTab),
    db(QSqlDatabase::database()),
    query(QSqlQuery(db))
{
    ui->setupUi(this);

    model = new QStandardItemModel(0, 5, this);
    model->setHeaderData(0, Qt::Horizontal, "Reservation ID");
    model->setHeaderData(1, Qt::Horizontal, "Check-In Date");
    model->setHeaderData(2, Qt::Horizontal, "Check-Out Date");
    model->setHeaderData(3, Qt::Horizontal, "Room Number");
    model->setHeaderData(4, Qt::Horizontal, "Guest Name");

    proxyModel = new MultiColumnFilterProxyModel(this);
    proxyModel->setSourceModel(model);
    proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    proxyModel->setFilterColumns({0, 3, 4}); // Searches Reservation ID, Room Number, Guest Name

    ui->ReservationsTabReservationsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->ReservationsTabReservationsTable->setModel(proxyModel);
    ui->ReservationsTabReservationsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    ui->ReservationsTabReservationsTable->setSelectionBehavior(QAbstractItemView::SelectRows);  // Clicking any cell highlights whole row
    ui->ReservationsTabReservationsTable->setSelectionMode(QAbstractItemView::SingleSelection); // User can only select one row at a time
    ui->ReservationsTabReservationsTable->horizontalHeader()->setSectionsClickable(false);      // User cannot click a column header to highlight all rows beneath it
    // Hiding the first column with row numbers
    ui->ReservationsTabReservationsTable->verticalHeader()->setVisible(false);

    connect(ui->ReservationsTabSearchfield, &QLineEdit::textChanged,
            proxyModel, &QSortFilterProxyModel::setFilterFixedString);

    connect(ui->ReservationsTabReservationsTable, &QTableView::doubleClicked,
            this, &ReservationsTab::handleViewReservation);

    connect(ui->ReservationTabEditButton, &QPushButton::clicked, this, &ReservationsTab::handleEditReservation);

    db.setDatabaseName("model.db");
    db.open();
    if (!db.open()) {
        qDebug() << "WARNING: ReservationsTab was unable to open the database";
        qDebug() << db.lastError().text();
    }

    // addMockData();
    populateReservationList(); // this reads all reservations from the database and adds them to the ui
    auto roomsOptional = parseRoomFile();
    if (roomsOptional.has_value()) {
        m_availableRooms = roomsOptional.value();
        qDebug() << "Geladen:" << m_availableRooms.size() << "kamers uit Excel";
    } else {
        qWarning() << "Kon Excel kamers niet laden!";
    }
}

ReservationsTab::~ReservationsTab()
{
    db.close();
    delete ui;
}

void ReservationsTab::onDataChanged()
{
    qDebug() << "Reservations Tab received notification that data changed. Refreshing UI...";
}

// EDIT BUTTON LOGIC
void ReservationsTab::handleEditReservation()
{
    QModelIndex proxyIndex = ui->ReservationsTabReservationsTable->currentIndex();

    if (!proxyIndex.isValid()) {
        QMessageBox::warning(this, "No Selection",
                             "Please click on a reservation first to edit it.");
        return;
    }

    Reservation currentRes = getReservationFromIndex(proxyIndex);

    EditReservationDialog dialog(this);

    // Eerst alle kamers inladen
    dialog.setAvailableRooms(m_availableRooms);

    // Daarna alle bestaande reserveringen meegeven
    dialog.setExistingReservations(getAllReservations());

    // ALS LAATSTE de huidige reservering instellen.
    // Hierdoor wordt de huidige kamer geselecteerd.
    dialog.setReservationData(currentRes);

    if (dialog.exec() == QDialog::Accepted) {

        Reservation updatedRes = dialog.getReservationData();

        QModelIndex sourceIndex = proxyModel->mapToSource(proxyIndex);
        int selectedRow = sourceIndex.row();

        model->item(selectedRow, 0)->setText(updatedRes.ReservationID);
        model->item(selectedRow, 1)->setText(
            updatedRes.checkIn.toString("dd/MM/yyyy"));
        model->item(selectedRow, 2)->setText(
            updatedRes.checkOut.toString("dd/MM/yyyy"));

        model->item(selectedRow, 3)->setData(
            updatedRes.roomNumber, Qt::DisplayRole);

        //model->item(selectedRow, 4)->setText(updatedRes.guest.guestID);

        model->item(selectedRow, 0)->setData(
            QVariant::fromValue(updatedRes), Qt::UserRole);

        updateReservationInDatabase(updatedRes);
        emit dataChangedNotification();
    }
}

void ReservationsTab::handleCancelReservation()
{
    QModelIndex proxyIndex = ui->ReservationsTabReservationsTable->currentIndex();

    if (!proxyIndex.isValid()) {
        QMessageBox::warning(this, "No Selection", "Please click on a reservation first to cancel it.");
        return;
    }

    Reservation res = getReservationFromIndex(proxyIndex);

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this,
                                  "Confirm Cancellation",
                                  QString("Are you sure you want to cancel reservation #%1 for %2?")
                                      .arg(res.ReservationID, res.guest.name),
                                  QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        // Clear table selection FIRST so Qt doesn't auto-trigger the adjacent row
        ui->ReservationsTabReservationsTable->clearSelection();
        ui->ReservationsTabReservationsTable->setCurrentIndex(QModelIndex());

        // Remove row from underlying source model
        QModelIndex sourceIndex = proxyModel->mapToSource(proxyIndex);
        model->removeRow(sourceIndex.row());

        emit dataChangedNotification();
    }
}

void ReservationsTab::addReservationToModel(const Reservation &res) {

    QList<QStandardItem*> row;

    QStandardItem *idItem = new QStandardItem(res.ReservationID);
    QStandardItem *checkInItem = new QStandardItem(res.checkIn.toString("dd/MM/yyyy"));
    QStandardItem *checkOutItem = new QStandardItem(res.checkOut.toString("dd/MM/yyyy"));
    QStandardItem *roomItem = new QStandardItem();
    roomItem->setData(res.roomNumber, Qt::DisplayRole);
    QStandardItem *nameItem = new QStandardItem(res.guest.name);

    // Saving the whole struct in Column 0
    idItem->setData(QVariant::fromValue(res), Qt::UserRole);

    row << idItem << checkInItem << checkOutItem << roomItem << nameItem;
    model->appendRow(row);


    if (!isReservationInDatabase(res)) {
        QSqlQuery nextrowid("SELECT COALESCE(MAX(rowid), 0) + 1 AS next_rowid FROM guests;");
        QString rowid;
        while (nextrowid.next()) {
            rowid = nextrowid.value(0).toString();
        }
        ReservationSerializer serializer;
        auto map = serializer.toVariantMap(res);
        query.prepare("insert into reservations values (:ReservationID, :GuestID, :NumberOfGuests, :RoomNumber, :CheckInDate, :CheckOutDate, :PaymentStatus);");
        query.bindValue(":ReservationID", map.value("reservation_id"));
        query.bindValue(":GuestID", rowid);
        query.bindValue(":NumberOfGuests", map.value("number_of_guests"));
        query.bindValue(":RoomNumber", map.value("room_number"));
        query.bindValue(":CheckInDate", map.value("check_in"));
        query.bindValue(":CheckOutDate", map.value("check_out"));
        query.bindValue(":PaymentStatus", map.value("payment_status"));
        query.exec();

        if (!isGuestInDatabase(res.guest)) {
            query.prepare("insert into guests values (:GuestID, :GuestName, :GuestPhoneNumber, :GuestEmail, :GuestAddress, :GuestDateOfBirth, :GuestGender, :GuestPaymentMethod, :GuestIdType);");
            query.bindValue(":GuestID", rowid);
            query.bindValue(":GuestName", map.value("guest_name"));
            query.bindValue(":GuestPhoneNumber", map.value("guest_phone"));
            query.bindValue(":GuestEmail", map.value("guest_email"));
            query.bindValue(":GuestAddress", map.value("address"));
            query.bindValue(":GuestDateOfBirth", map.value("date_of_birth"));
            query.bindValue(":GuestGender", map.value("gender"));
            query.bindValue(":GuestPaymentMethod", map.value("payment_method"));
            query.bindValue(":GuestIdType", map.value("id_type"));
            query.exec();
        }
    }
}

Reservation ReservationsTab::getReservationFromIndex(const QModelIndex &index)
{
    // Map from proxy index back to source index
    QModelIndex sourceIndex = proxyModel->mapToSource(index);

    // Get item from Column 0 of that row
    QStandardItem *item = model->item(sourceIndex.row(), 0);

    // Retrieve full Reservation struct stored in Qt::UserRole
    return item->data(Qt::UserRole).value<Reservation>();
}

QList<Reservation> ReservationsTab::getAllReservations()
{
    ReservationSerializer serializer;
    QList<Reservation> reservations;
    QSqlQuery resQuery("SELECT * FROM reservations;");
    QSqlQuery guestQuery("SELECT * FROM guests;");
    QSqlQuery testQuery("SELECT GuestID FROM reservations;");
    while(testQuery.next()){
        QString name = testQuery.value(0).toString();
        qDebug() << name;
    }

    int resIdIndex = resQuery.record().indexOf("ReservationID");
    int guestIdIndex = resQuery.record().indexOf("GuestID");
    int numberOfGuestsIndex = resQuery.record().indexOf("NumberOfGuests");
    int roomNumberIndex = resQuery.record().indexOf("RoomNumber");
    int checkInIndex = resQuery.record().indexOf("CheckInDate");
    int checkOutIndex = resQuery.record().indexOf("CheckOutDate");
    int paymentStatusIndex = resQuery.record().indexOf("PaymentStatus");

    int IdIndex = guestQuery.record().indexOf("GuestID");
    int guestNameIndex = guestQuery.record().indexOf("GuestName");
    int guestPhoneNumberIndex = guestQuery.record().indexOf("GuestPhoneNumber");
    int guestEmailIndex = guestQuery.record().indexOf("GuestEmail");
    int guestAddressIndex = guestQuery.record().indexOf("GuestAddress");
    int guestDateOfBirthIndex = guestQuery.record().indexOf("GuestDateOfBirth");
    int guestGenderIndex = guestQuery.record().indexOf("GuestGender");
    int guestPaymentMethodIndex = guestQuery.record().indexOf("GuestPaymentMethod");
    int guestIdTypeIndex = guestQuery.record().indexOf("GuestIdType");

    while (resQuery.next()) {
        QVariantMap resMap;
        resMap[QStringLiteral("reservation_id")] = resQuery.value(resIdIndex).toString();
        resMap[QStringLiteral("number_of_guests")] = resQuery.value(numberOfGuestsIndex).toString();
        resMap[QStringLiteral("room_number")] = resQuery.value(roomNumberIndex).toString();
        resMap[QStringLiteral("check_in")] = resQuery.value(checkInIndex).toString();
        resMap[QStringLiteral("check_out")] = resQuery.value(checkOutIndex).toString();
        resMap[QStringLiteral("payment_status")] = resQuery.value(paymentStatusIndex).toString();

        guestQuery.prepare("SELECT * FROM guests where GuestID = :id;");
        guestQuery.bindValue(":id", resQuery.value(guestIdIndex).toString());
        guestQuery.exec();
        if (guestQuery.next()) {
            resMap[QStringLiteral("guest_id")] = guestQuery.value(IdIndex).toString();
            resMap[QStringLiteral("guest_name")] = guestQuery.value(guestNameIndex).toString();
            resMap[QStringLiteral("guest_phone")] = guestQuery.value(guestPhoneNumberIndex).toString();
            resMap[QStringLiteral("guest_email")] = guestQuery.value(guestEmailIndex).toString();
            resMap[QStringLiteral("address")] = guestQuery.value(guestAddressIndex).toString();
            resMap[QStringLiteral("date_of_birth")] = guestQuery.value(guestDateOfBirthIndex).toString();
            resMap[QStringLiteral("gender")] = guestQuery.value(guestGenderIndex).toString();
            resMap[QStringLiteral("payment_method")] = guestQuery.value(guestPaymentMethodIndex).toString();
            resMap[QStringLiteral("id_type")] = guestQuery.value(guestIdTypeIndex).toString();
        }

        // qDebug() << selectQuery.value(0).toString();
        reservations.append(serializer.fromVariantMap(resMap));
    }

    // for (int row = 0; row < model->rowCount(); ++row) {
    //     QStandardItem *item = model->item(row, 0);

    //     if (item) {
    //         reservations.append(
    //             item->data(Qt::UserRole).value<Reservation>()
    //             );
    //     }
    // }

    return reservations;
}

bool ReservationsTab::isReservationInDatabase(const Reservation &res) {
    bool status = false;
    query.prepare("SELECT ReservationID FROM reservations WHERE ReservationID = :id;");
    query.bindValue(":id", res.ReservationID);
    if (query.exec()) {
        if (query.next()) {
            status = true;
        }
    }
    // we can potentially add more uniqueness checks later
    return status;
}

bool ReservationsTab::isGuestInDatabase(const Guest &guest) {
    bool status = false;
    query.prepare("select GuestID from guests WHERE GuestID = :id;");
    query.bindValue(":id", guest.guestID);
    if (query.exec()) {
        if (query.next()) {
            status = true;
        }
    }
    // we can potentially add more uniqueness checks later
    //return status;
    return false; //TODO this function does nothing for now because there's not currently a way to assign a reservation to an existing guest
}

void ReservationsTab::updateReservationInDatabase(const Reservation &res) {
    QSqlQuery resQuery(db);
    QSqlQuery guestQuery(db);
    if (!isReservationInDatabase(res)) {
        qDebug() << "Reservation cannot be edited because it does not exist!";
        return;
    }
    auto map = ReservationSerializer::toVariantMap(res);
    resQuery.prepare("UPDATE reservations SET GuestID = :GuestID, NumberOfGuests = :NumberOfGuests, RoomNumber = :RoomNumber, CheckInDate = :CheckInDate, CheckOutDate = :CheckOutDate, PaymentStatus = :PaymentStatus WHERE ReservationID = :id;");
    resQuery.bindValue(":GuestID", map.value("guest_id"));
    resQuery.bindValue(":NumberOfGuests", map.value("number_of_guests"));
    resQuery.bindValue(":RoomNumber", map.value("room_number"));
    resQuery.bindValue(":CheckInDate", map.value("check_in"));
    resQuery.bindValue(":CheckOutDate", map.value("check_out"));
    resQuery.bindValue(":PaymentStatus", map.value("payment_status"));
    resQuery.bindValue(":id", map.value("reservation_id"));
    bool status = resQuery.exec();
    if (!status) {
        qDebug() << "Error updating reservation: " << resQuery.lastError().text();
    }

    guestQuery.prepare("UPDATE guests SET GuestName = :GuestName, GuestPhoneNumber = :GuestPhoneNumber, GuestEmail = :GuestEmail, GuestAddress = :GuestAddress, GuestDateOfBirth = :GuestDateOfBirth, GuestGender = :GuestGender, GuestPaymentMethod = :GuestPaymentMethod, GuestIdType = :GuestIdType WHERE GuestID = :id;");
    guestQuery.bindValue(":id", map.value("guest_id"));
    guestQuery.bindValue(":GuestName", map.value("guest_name"));
    guestQuery.bindValue(":GuestPhoneNumber", map.value("guest_phone"));
    guestQuery.bindValue(":GuestEmail", map.value("guest_email"));
    guestQuery.bindValue(":GuestAddress", map.value("address"));
    guestQuery.bindValue(":GuestDateOfBirth", map.value("date_of_birth"));
    guestQuery.bindValue(":GuestGender", map.value("gender"));
    guestQuery.bindValue(":GuestPaymentMethod", map.value("payment_method"));
    guestQuery.bindValue(":GuestIdType", map.value("id_type"));

    bool guestStatus = guestQuery.exec();
    if (!guestStatus) {
        qDebug() << "Error updating guest: " << guestQuery.lastError().text();
    }
}

// Placeholder method for adding mock data - REMOVE LATER
void ReservationsTab::addMockData()
{
    Reservation r1;
    r1.ReservationID = "1";
    r1.guest.name = "Giovanni Falegname";
    r1.guest.phoneNumber = "31 6 12345678";
    r1.guest.address = {"Maasdijk 68", "", "Veen", "Noord-Brabant", "4356AB", "Netherlands"};
    r1.guest.dateOfBirth = QDate(1991, 10, 1);
    r1.guest.gender = Gender::Male;
    r1.numberOfGuests = 2;
    r1.roomNumber = "1.01";
    r1.checkIn = QDate(2026, 8, 12);
    r1.checkOut = QDate(2026, 8, 15);

    Reservation r2;
    r2.ReservationID = "2";
    r2.guest.name = "Gualtiero Gaykemani";
    r2.guest.phoneNumber = "31 6 12345678";
    r2.guest.address = {"Maasdijk 69", "", "Veen", "Noord-Brabant", "4356AB", "Netherlands"};
    r2.guest.dateOfBirth = QDate(1992, 10, 1);
    r2.guest.gender = Gender::Male;
    r2.numberOfGuests = 3;
    r2.roomNumber = "1.02";
    r2.checkIn = QDate(2026, 9, 12);
    r2.checkOut = QDate(2026, 9, 15);

    addReservationToModel(r1);
    addReservationToModel(r2);
}

void ReservationsTab::populateReservationList() {
    QDate today = QDate::currentDate();

    for (const auto &res : getAllReservations()) {
        if (res.checkOut < today) {
            continue;
        }
        addReservationToModel(res);
    }
}

void ReservationsTab::handleViewReservation(const QModelIndex &index)
{
    if (!index.isValid())
        return;

    Reservation res = getReservationFromIndex(index);

    EditReservationDialog dialog(this);

    dialog.setAvailableRooms(m_availableRooms);
    dialog.setExistingReservations(getAllReservations());
    dialog.setReservationData(res);
    dialog.setReadOnly(false);

    if (dialog.exec() == QDialog::Accepted) {

        Reservation updatedRes = dialog.getReservationData();
        QModelIndex sourceIndex = proxyModel->mapToSource(index);
        int selectedRow = sourceIndex.row();

        model->item(selectedRow, 0)->setText(updatedRes.ReservationID);
        model->item(selectedRow, 1)->setText(
            updatedRes.checkIn.toString("dd/MM/yyyy"));
        model->item(selectedRow, 2)->setText(
            updatedRes.checkOut.toString("dd/MM/yyyy"));
        model->item(selectedRow, 3)->setData(
            updatedRes.roomNumber, Qt::DisplayRole);
        model->item(selectedRow, 4)->setText(updatedRes.guest.name);

        model->item(selectedRow, 0)->setData(
            QVariant::fromValue(updatedRes), Qt::UserRole);

        updateReservationInDatabase(updatedRes);
        emit dataChangedNotification();
    }
}


void ReservationsTab::on_ReservationsTabNewReservationButton_clicked()
{
    EditReservationDialog dialog(this);

    Reservation newRes;
    newRes.checkIn = QDate::currentDate();
    newRes.checkOut = QDate::currentDate().addDays(1);
    newRes.guest.dateOfBirth = QDate::currentDate().addYears(-20);
    newRes.numberOfGuests = 1;
    newRes.roomNumber = 0;

    dialog.setAvailableRooms(m_availableRooms);
    dialog.setExistingReservations(getAllReservations());
    dialog.setReservationData(newRes);

    dialog.setWindowTitle("New Reservation");

    if (dialog.exec() == QDialog::Accepted) {
        // Retrieve filled struct from dialog
        newRes = dialog.getReservationData();


        //this adds ID to table
        QSqlQuery nextrowid("SELECT COALESCE(MAX(rowid), 0) + 1 AS next_rowid FROM reservations;");
        QString rowid;
        while (nextrowid.next()) {
            rowid = nextrowid.value(0).toString();
        }
        newRes.ReservationID = rowid;

        // Add to table model
        addReservationToModel(newRes);

        emit dataChangedNotification();
    }

}

