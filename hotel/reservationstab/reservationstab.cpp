#include "reservationstab.h"
#include "ui_reservationstab.h"
#include "editreservationdialog.h"
#include "../../readers/reservationserializer.h"

#include <QMessageBox>
#include <iostream>
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

    proxyModel = new QSortFilterProxyModel(this);
    proxyModel->setSourceModel(model);
    proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    proxyModel->setFilterKeyColumn(-1);  // Search all columns

    ui->ReservationsTabReservationsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->ReservationsTabReservationsTable->setModel(proxyModel);
    ui->ReservationsTabReservationsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    connect(ui->ReservationsTabSearchfield, &QLineEdit::textChanged,
            proxyModel, &QSortFilterProxyModel::setFilterFixedString);

    connect(ui->ReservationsTabReservationsTable, &QTableView::doubleClicked,
            this, &ReservationsTab::handleViewReservation);

    connect(ui->ReservationTabEditButton, &QPushButton::clicked, this, &ReservationsTab::handleEditReservation);
    connect(ui->ReservationTabCancelButton, &QPushButton::clicked, this, &ReservationsTab::handleCancelReservation);

    db.setDatabaseName("model.db");
    db.open();
    if (!db.open()) {
        qDebug() << "WARNING: ReservationsTab was unable to open the database";
        qDebug() << db.lastError().text();
    }

    // addMockData();
    populateReservationList();
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

        model->item(selectedRow, 4)->setText(updatedRes.GuestName);

        model->item(selectedRow, 0)->setData(
            QVariant::fromValue(updatedRes), Qt::UserRole);

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
                                      .arg(res.ReservationID, res.GuestName),
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
    QStandardItem *nameItem = new QStandardItem(res.GuestName);

    // Saving the whole struct in Column 0
    idItem->setData(QVariant::fromValue(res), Qt::UserRole);

    row << idItem << checkInItem << checkOutItem << roomItem << nameItem;
    model->appendRow(row);

    if (!isReservationInDatabase(res)) {
        ReservationSerializer serializer;
        auto map = serializer.toVariantMap(res);
        query.prepare("insert into reservations values (:reservationID, :name, :phoneNumber, :address, :dateOfBirth, :gender, :numberOfGuests, :roomNumber, :checkInDate, :checkOutDate);");
        query.bindValue(":reservationID", map.value("reservation_id"));
        query.bindValue(":name", map.value("guest_name"));
        query.bindValue(":phoneNumber", map.value("guest_phone"));
        query.bindValue(":address", map.value("address"));
        query.bindValue(":dateOfBirth", map.value("date_of_birth"));
        query.bindValue(":gender", map.value("gender")); // temporary placeholder until a .toString() is implemented for gender
        query.bindValue(":numberOfGuests", map.value("number_of_guests"));
        query.bindValue(":roomNumber", map.value("room_number"));
        query.bindValue(":checkInDate", map.value("check_in"));
        query.bindValue(":checkOutDate", map.value("check_out"));
        query.exec();
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
    QSqlQuery selectQuery("SELECT * FROM reservations;");
    int indexID = query.record().indexOf("ReservationID");
    int indexName = query.record().indexOf("GuestName");
    int indexPhoneNumber = query.record().indexOf("GuestPhoneNumber");
    int indexAddress = query.record().indexOf("Address");
    int indexDateOfBirth = query.record().indexOf("DateOfBirth");
    int indexGender = query.record().indexOf("Gender");
    int indexNumberOfGuests = query.record().indexOf("NumberOfGuests");
    int indexRoomNumber = query.record().indexOf("RoomNumber");
    int indexCheckIn = query.record().indexOf("CheckInDate");
    int indexCheckOut = query.record().indexOf("CheckOutDate");

    while (selectQuery.next()) {
        QVariantMap resMap;
        resMap[QStringLiteral("reservation_id")] = selectQuery.value(0).toString();
        resMap[QStringLiteral("guest_name")] = selectQuery.value(1).toString();
        resMap[QStringLiteral("guest_phone")] = selectQuery.value(2).toString();
        resMap[QStringLiteral("address")] = selectQuery.value(3).toString();
        resMap[QStringLiteral("date_of_birth")] = selectQuery.value(4).toString();
        resMap[QStringLiteral("gender")] = selectQuery.value(5).toString();
        resMap[QStringLiteral("number_of_guests")] = selectQuery.value(6).toString();
        resMap[QStringLiteral("room_number")] = selectQuery.value(7).toString();
        resMap[QStringLiteral("check_in")] = selectQuery.value(8).toString();
        resMap[QStringLiteral("check_out")] = selectQuery.value(9).toString();

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

// Placeholder method for adding mock data - REMOVE LATER
void ReservationsTab::addMockData()
{
    Reservation r1;
    r1.ReservationID = "1";
    r1.GuestName = "Giovanni Falegname";
    r1.GuestPhoneNumber = "31 6 12345678";
    r1.address = {"Maasdijk 68", "", "Veen", "Noord-Brabant", "4356AB", "Netherlands"};
    r1.dateOfBirth = QDate(1991, 10, 1);
    r1.gender = Gender::Male;
    r1.numberOfGuests = 2;
    r1.roomNumber = 101;
    r1.checkIn = QDate(2026, 8, 12);
    r1.checkOut = QDate(2026, 8, 15);

    Reservation r2;
    r2.ReservationID = "2";
    r2.GuestName = "Gualtiero Gaykemani";
    r2.GuestPhoneNumber = "31 6 12345678";
    r2.address = {"Maasdijk 69", "", "Veen", "Noord-Brabant", "4356AB", "Netherlands"};
    r2.dateOfBirth = QDate(1992, 10, 1);
    r2.gender = Gender::Male;
    r2.numberOfGuests = 3;
    r2.roomNumber = 102;
    r2.checkIn = QDate(2026, 9, 12);
    r2.checkOut = QDate(2026, 9, 15);

    addReservationToModel(r1);
    addReservationToModel(r2);

    // QList<QStandardItem*> row2;
    // row2 << new QStandardItem("0002")
    //      << new QStandardItem("Giovane Principe")
    //      << new QStandardItem("111")
    //      << new QStandardItem("14/07/2026")
    //      << new QStandardItem("20/07/2026");
    // model->appendRow(row2);

    // QList<QStandardItem*> row4;
    // row4 << new QStandardItem("0004")
    //      << new QStandardItem("Gioacchino della Pietra")
    //      << new QStandardItem("108")
    //      << new QStandardItem("10/06/2026")
    //      << new QStandardItem("20/06/2026");
    // model->appendRow(row4);

    // QList<QStandardItem*> row5;
    // row5 << new QStandardItem("0005")
    //      << new QStandardItem("Giovanni Pialla")
    //      << new QStandardItem("201")
    //      << new QStandardItem("08/08/2026")
    //      << new QStandardItem("18/08/2026");
    // model->appendRow(row5);
}

void ReservationsTab::populateReservationList() {
    for (auto res : getAllReservations()) {
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
        model->item(selectedRow, 4)->setText(updatedRes.GuestName);

        model->item(selectedRow, 0)->setData(
            QVariant::fromValue(updatedRes), Qt::UserRole);

        emit dataChangedNotification();
    }
}


void ReservationsTab::on_ReservationsTabNewReservationButton_clicked()
{
    EditReservationDialog dialog(this);

    Reservation newRes;
    newRes.checkIn = QDate::currentDate();
    newRes.checkOut = QDate::currentDate().addDays(1);
    newRes.dateOfBirth = QDate::currentDate().addYears(-20);
    newRes.numberOfGuests = 1;
    newRes.roomNumber = 0;

    dialog.setAvailableRooms(m_availableRooms);
    dialog.setExistingReservations(getAllReservations());
    dialog.setReservationData(newRes);

    if (dialog.exec() == QDialog::Accepted) {
        // Retrieve filled struct from dialog
        newRes = dialog.getReservationData();

        // Generate a temporary ID
        newRes.ReservationID = QString::number(model->rowCount() + 1);

        // Add to table model
        addReservationToModel(newRes);

        emit dataChangedNotification();
    }

}

