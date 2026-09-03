#include "reservationstab.h"
#include "ui_reservationstab.h"
#include "editreservationdialog.h"

#include <QMessageBox>
#include <iostream>
#include <QDate>
#include <QDebug>
#include <QString>

ReservationsTab::ReservationsTab(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ReservationsTab)
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
    connect(ui->ReservationTabCancelButton, &QPushButton::clicked, this, &ReservationsTab::handleCancelReservation);

    // For now - REMOVE LATER:
    addMockData();
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

        model->item(selectedRow, 4)->setText(updatedRes.guest.name);

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

QList<Reservation> ReservationsTab::getAllReservations() const
{
    QList<Reservation> reservations;

    for (int row = 0; row < model->rowCount(); ++row) {
        QStandardItem *item = model->item(row, 0);

        if (item) {
            reservations.append(
                item->data(Qt::UserRole).value<Reservation>()
                );
        }
    }

    return reservations;
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

        // Generate a temporary ID
        newRes.ReservationID = QString::number(model->rowCount() + 1);

        // Add to table model
        addReservationToModel(newRes);

        emit dataChangedNotification();
    }

}

