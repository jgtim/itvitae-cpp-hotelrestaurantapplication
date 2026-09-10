#include "roomswindow.h"
#include "ui_roomswindow.h"
#include "confirmdialog.h"
#include "Room/room.h"
#include "../../readers/roomrepositoryfactory.h"
#include "../../readers/roomserializer.h"

#include <QStandardItemModel>
#include <QStandardItem>
#include <QColor>
#include <QBrush>
#include <QSortFilterProxyModel>
#include <QHeaderView>
#include <QMessageBox>
#include <QSqlQuery>


// #include <QSqlTableModel> -- Later voor SQL aanzetten

//TEST

RoomsWindow::RoomsWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::RoomsWindow)
    , roomRepository(createRoomRepository())
    , db(QSqlDatabase::database())
{
    ui->setupUi(this);

    model = new QStandardItemModel(this);
    ui->roomStack->setCurrentIndex(0);
    ui->CancelButton->setVisible(false);
    ui->RoomNumEdit->setValidator(new QDoubleValidator(0, 100, 2, this));
    ui->CapacityEdit->setValidator(new QIntValidator(0, 100, this));

    QStringList roomTypes = { "Economy", "Standard", "Luxurious", "Unknown" };
    ui->RoomTypeEdit->addItems(roomTypes);

    QStringList roomStatuses = { "Available", "Occupied", "Reserved", "Maintenance" };
    ui->StatusEdit->addItems(roomStatuses);

    QStringList Cleaningstatuses = { "Clean", "Dirty" };
    ui->CleaningEdit->addItems(Cleaningstatuses);

    // --- later voor SQL aanzetten ---
    //QSqlTableModel *model = new QSqlTableModel(this);

    // model->setTable("Rooms");
    // model->select();

    model->setHorizontalHeaderLabels({
        //NOTE IF YOU CHANGE ANYTHING: these names are used in on_seeRoomDetails_clicked()
        "Room Number",
        "Room type",
        "Capacity",
        "View",        //spaties om het vakje groter te maken
        "Babybed",
        "Bed type",
        "Comments", //spaties om het vakje groter te maken
        "Price per night",
        "Status",
        "Housekeeping",
        //"More Information" //for if we add button here later
    });


    loadRooms();
    proxyModel = new MultiColumnFilterProxyModel(this);
    proxyModel->setSourceModel(model);
    proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    proxyModel->setFilterColumns({0,1,3,8,9});

    ui->roomsTableView->setModel(proxyModel);
    ui->roomsTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->roomsTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    ui->roomsTableView->setSelectionBehavior(QAbstractItemView::SelectRows);  // Clicking any cell highlights whole row
    ui->roomsTableView->setSelectionMode(QAbstractItemView::SingleSelection); // User can only select one row at a time
    ui->roomsTableView->horizontalHeader()->setSectionsClickable(false);      // User cannot click a column header to highlight all rows beneath it

    // Hiding the first column with row numbers
    ui->roomsTableView->verticalHeader()->setVisible(false);

    connect(ui->roomSearchLineEdit,
            &QLineEdit::textChanged,
            this,
            [this](const QString &text)
            {
                proxyModel->setFilterFixedString(text);
            });

    updateRoomColors();
}

RoomsWindow::~RoomsWindow()
{
    delete ui;
}

//Ik heb functie gescrheven zodat kleuren worden toegekend a.d.h.v. statusnaam.
void RoomsWindow::updateRoomColors()
{
    for (int row = 0; row < model->rowCount(); row++)
    {
        // Status kolom = 8

        QStandardItem *statusItem = model->item(row, 8);

        if (statusItem)
        {
            statusItem->setForeground(QBrush(QColor(0, 0, 0)));
            QString status = statusItem->text();

            if (status == "Available")
                statusItem->setBackground(QColor(200, 255, 200));
            else if (status == "Occupied")
                statusItem->setBackground(QColor(255, 200, 200));
            else if (status == "Reserved")
                statusItem->setBackground(QColor(200, 220, 255));
            else if (status == "Maintenance")
                statusItem->setBackground(QColor(200, 200, 200));
        }


        // Housekeeping kolom = 9
        QStandardItem *housekeepingItem = model->item(row, 9);
        if (housekeepingItem)
        {
            housekeepingItem->setForeground(QBrush(QColor(0, 0, 0)));
            QString housekeeping = housekeepingItem->text();

            // Gebruikt nu de enum in plaats van de string literal om te checken
            if (housekeeping == houseKeepingStatusToString(HouseKeepingStatus::Clean))
                housekeepingItem->setBackground(QColor(200, 255, 200));
            else if (housekeeping == houseKeepingStatusToString(HouseKeepingStatus::Dirty))
                housekeepingItem->setBackground(QColor(255, 200, 200));
        }
    }
}

// Writes a single Room's fields into the given row of the display model.
// Shared by loadRooms() (initial fill) and saveRoomData() (refresh after an edit) so the
// two never drift out of sync with each other.
void RoomsWindow::populateModelRow(int row, const Room &room) {
    // Column 0: Room Number
    model->setItem(row, 0, new QStandardItem(room.roomNumber));
    // Column 1: Room Type (Enum -> String)
    model->setItem(row, 1, new QStandardItem(roomTypeToString(room.roomType)));
    // Column 2: Adults
    model->setItem(row, 2, new QStandardItem(QString::number(room.adults)));
    // Column 3: Room View (Enum -> String)
    model->setItem(row, 3, new QStandardItem(roomViewToString(room.roomView)));
    // Column 4: Baby Bed (Bool -> String)
    model->setItem(row, 4, new QStandardItem(room.babyBed ? "Yes" : "No"));
    // Column 5: Bed Type
    model->setItem(row, 5, new QStandardItem(room.beds));
    // Column 6: Comments
    model->setItem(row, 6, new QStandardItem(room.comments));
    // Column 7: Price per Night (2 decimal places)
    QString priceString = QString("€ %1").arg(room.pricePerNight, 0, 'f', 2);
    model->setItem(row, 7, new QStandardItem(priceString));
    // Column 8: Room Status
    model->setItem(row, 8, new QStandardItem(roomStatusToString(room.roomStatus)));
    // Column 9: Housekeeping Status
    model->setItem(row, 9, new QStandardItem(houseKeepingStatusToString(room.houseKeepingStatus)));
}

void RoomsWindow::loadRooms(){
    std::optional<QList<Room>> roomList = roomRepository->loadRooms();

    if (!roomList.has_value()) {
        qDebug() << "Could not import room data";
        QMessageBox::warning(this, "Import Error", "Failed to load room data.");
        return;
    }

    rooms = *roomList;

    model->removeRows(0, model->rowCount()); // Clear any existing items

    for (int i = 0; i < rooms.size(); ++i) {
        populateModelRow(i, rooms.at(i));
        if (!isRoomInDatabase(rooms.at(i))) {
            addRoomToDatabase(rooms.at(i)); // if the room isn't in the database, add it
        } else {
            updateRoomPrice(rooms.at(i)); // otherwise, just update the price
        }
    }
}

void RoomsWindow::addRoomToDatabase(const Room& room) {
    QSqlQuery roomQuery(db);

    if (!isRoomInDatabase(room)) {
        auto map = RoomSerializer::toVariantMap(room);

        roomQuery.prepare("INSERT INTO rooms VALUES (:RoomNumber, :RoomType, :Adults, :RoomView, :BabyBed, :Beds, :Comments, :PricePerNight, :RoomStatus, :HousekeepingStatus);");

        roomQuery.bindValue(":RoomNumber", map.value("room_number"));
        roomQuery.bindValue(":RoomType", map.value("room_type"));
        roomQuery.bindValue(":Adults", map.value("adults"));
        roomQuery.bindValue(":RoomView", map.value("room_view"));
        roomQuery.bindValue(":BabyBed", map.value("baby_bed"));
        roomQuery.bindValue(":Beds", map.value("beds"));
        roomQuery.bindValue(":Comments", map.value("comments"));
        roomQuery.bindValue(":PricePerNight", map.value("price_per_night"));
        roomQuery.bindValue(":RoomStatus", map.value("room_status"));
        roomQuery.bindValue(":HousekeepingStatus", map.value("housekeeping_status"));
        roomQuery.exec();
    }
}

bool RoomsWindow::isRoomInDatabase(const Room& room) {
    QSqlQuery roomQuery(db);
    bool status = false;

    roomQuery.prepare("SELECT RoomNumber FROM rooms WHERE RoomNumber = :roomNum;");
    roomQuery.bindValue(":roomNum", room.roomNumber);
    if (roomQuery.exec()) {
        if (roomQuery.next()) {
            status = true;
        }
    }
    return status;
}

void RoomsWindow::updateRoomInDatabase(const Room& room) {
    auto map = RoomSerializer::toVariantMap(room);
    QSqlQuery roomQuery(db);

    roomQuery.prepare("UPDATE rooms SET RoomType = :RoomType, Adults = :Adults, RoomView = :RoomView, BabyBed = :BabyBed, Beds = :Beds, Comments = :Comments, RoomStatus = :RoomStatus, HousekeepingStatus = :HousekeepingStatus WHERE RoomNumber = :RoomNumber;");

    roomQuery.bindValue(":RoomType", map.value("room_type").toString());
    roomQuery.bindValue(":Adults", map.value("adults").toString());
    roomQuery.bindValue(":RoomView", map.value("room_view").toString());
    roomQuery.bindValue(":BabyBed", map.value("baby_bed").toString());
    roomQuery.bindValue(":Beds", map.value("beds").toString());
    roomQuery.bindValue(":Comments", map.value("comments").toString());
    roomQuery.bindValue(":RoomStatus", map.value("room_status").toString());
    roomQuery.bindValue(":HousekeepingStatus", map.value("housekeeping_status").toString());

    roomQuery.bindValue(":RoomNumber", map.value("room_number").toString());

    roomQuery.exec();
}

void RoomsWindow::updateRoomPrice(const Room& room) {
    auto map = RoomSerializer::toVariantMap(room);
    QSqlQuery roomQuery(db);

    roomQuery.prepare("UPDATE rooms SET PricePerNight = :PricePerNight WHERE RoomNumber = :RoomNumber;");

    roomQuery.bindValue(":PricePerNight", map.value("price_per_night"));
    roomQuery.bindValue(":RoomNumber", map.value("room_number"));

    roomQuery.exec();
}

Room RoomsWindow::getRoomFromDatabaseByNumber(const QString& roomNumber){
    QSqlQuery roomQuery(db);
    QVariantMap roomMap;
    roomQuery.prepare("SELECT RoomNumber, RoomType, Beds, RoomView, BabyBed, Comments, PricePerNight, RoomStatus, HousekeepingStatus FROM rooms WHERE RoomNumber = :roomNum;");

    roomQuery.bindValue(":roomNum", roomNumber);
    if (roomQuery.exec()) {
        if (roomQuery.next()) {
            roomMap["room_number"] = roomQuery.value("RoomNumber").toString();
            roomMap["room_type"] = roomQuery.value("RoomType").toString();
            roomMap["capacity"] = roomQuery.value("Beds").toString();
            roomMap["room_view"] = roomQuery.value("RoomView").toString();
            roomMap["price_per_night"] = roomQuery.value("PricePerNight").toDouble();
            roomMap["room_status"] = roomQuery.value("RoomStatus").toString();
            roomMap["housekeeping_status"] = roomQuery.value("HousekeepingStatus").toString();
        }
    }
    // not finished yet, need to fix room implemenation in the database first
}

void RoomsWindow::on_roomsTableView_doubleClicked(const QModelIndex &proxyIndex)
{

    on_seeRoomDetails_clicked();
    //this chunk of code creates a roomdetailsdialog. this has been replaced.
    // if (!proxyIndex.isValid())
    //     return;

    // QModelIndex sourceIndex = proxyModel->mapToSource(proxyIndex);
    // int selectedRow = sourceIndex.row();


    // QList<QPair<QString, QString>> roomDetails;
    // int columnCount = model->columnCount();

    // for (int col = 0; col < columnCount; ++col) {
    //     QString header = model->horizontalHeaderItem(col)->text();

    //     QStandardItem *item = model->item(selectedRow, col);
    //     QString value = item ? item->text() : "";

    //     roomDetails.append(qMakePair(header, value));
    // }

    // RoomDetailsDialog dialog(this);
    // dialog.setDetails(roomDetails);
    // if (dialog.exec() == QDialog::Accepted) {
    //     QList<QPair<QString, QString>> updatedDetails = dialog.getDetails();
    //     for (int col = 0; col < updatedDetails.size(); ++col) {
    //         QStandardItem *item = model->item(selectedRow, col);
    //         if (item) {
    //             item->setText(updatedDetails[col].second);
    //         }
    //     }
    // }
}

void RoomsWindow::fillInRoomData(int row){
    int columnCount = model->columnCount();

    for (int col = 0; col < columnCount; ++col) {
        setEditMode(true);

        QString header = model->horizontalHeaderItem(col)->text();
        QStandardItem *item = model->item(row, col);
        QString value = item ? item->text() : "";
        if(header == "Room Number"){
            ui->RoomNumEdit->setText(value);
        }else if(header == "Room type"){
            int index = ui->RoomTypeEdit->findText(value);
            ui->RoomTypeEdit->setCurrentIndex(index);
        }else if(header == "Capacity"){
            ui->CapacityEdit->setText(value);
        }else if(header == "Status"){
            int index = ui->StatusEdit->findText(value);
            ui->StatusEdit->setCurrentIndex(index);
        }else if(header == "Housekeeping"){
            int index = ui->CleaningEdit->findText(value);
            ui->CleaningEdit->setCurrentIndex(index);
        }else if(header == "Comments"){
            ui->NotesEdit->setText(value);
        }else if(header == "Comments        "){
            ui->NotesEdit->setPlainText(value);
        }
    }
}

void RoomsWindow::saveRoomData(int row){
    if (row < 0 || row >= rooms.size()) return;

    // Build the updated Room on a copy first. Fields the UI doesn't expose for editing
    // view, baby bed, beds and price are simply left as they were.
    Room updatedRoom = rooms[row];
    updatedRoom.roomNumber = ui->RoomNumEdit->text();
    updatedRoom.roomType = stringToRoomType(ui->RoomTypeEdit->currentText());
    updatedRoom.adults = ui->CapacityEdit->text().toInt();
    updatedRoom.roomStatus = stringToRoomStatus(ui->StatusEdit->currentText());
    updatedRoom.houseKeepingStatus = stringToHouseKeepingStatus(ui->CleaningEdit->currentText());
    updatedRoom.comments = ui->NotesEdit->toPlainText();

    // Try to persist the copy first. Only commit it to rooms/model once storage confirms the save succeeded
    QList<Room> attemptedRooms = rooms;
    attemptedRooms[row] = updatedRoom;

    if (!roomRepository->saveRooms(attemptedRooms)) {
        QMessageBox::warning(this, "Save Error", "Failed to save room data to storage. Your changes have not been saved.");
        return;
    }

    rooms = attemptedRooms;
    populateModelRow(row, updatedRoom); // Reflect the change in the table now that it's safely saved
    updateRoomColors();                 // status/housekeeping colors need refreshing after a save
}

void RoomsWindow::on_seeRoomDetails_clicked()
{


    QModelIndex proxyIndex = ui->roomsTableView->currentIndex();

    if (!proxyIndex.isValid()) {
        QMessageBox::warning(this, "No Selection", "Please select a room first to view and edit it.");
        return;
    }
    QModelIndex sourceIndex = proxyModel->mapToSource(proxyIndex);
    int selectedRow = sourceIndex.row();
    fillInRoomData(selectedRow);

    ui->roomStack->setCurrentIndex(1);
}


void RoomsWindow::on_ReturnButton_clicked()
{
    bool editing = ui->CancelButton->isVisible();
    if(editing){
        ConfirmDialog dialog(this);
        dialog.changeText("Leave without saving changes?");
        if (dialog.exec() == QDialog::Rejected) {
            return;
        }
    }
    ui->roomStack->setCurrentIndex(0);
}

void RoomsWindow::setEditMode(bool readMode){
    const QString newEditButtonText = (readMode) ? "Edit" : "Save";
    ui->EditButton->setText(newEditButtonText);
    ui->CancelButton->setVisible(!readMode);
    ui->RoomNumEdit->setReadOnly(readMode);
    ui->RoomTypeEdit->setDisabled(readMode);
    ui->CapacityEdit->setReadOnly(readMode);
    ui->StatusEdit->setDisabled(readMode);
    ui->CleaningEdit->setDisabled(readMode);
    ui->NotesEdit->setReadOnly(readMode);
}


void RoomsWindow::on_EditButton_clicked()
{
    bool editingNow =  ui->CancelButton->isVisible();
    if(editingNow){
        ConfirmDialog dialog(this);
        dialog.changeText("Save changes?");
        if (dialog.exec() == QDialog::Rejected) {
            return;
        }else{
            QModelIndex proxyIndex = ui->roomsTableView->currentIndex();
            QModelIndex sourceIndex = proxyModel->mapToSource(proxyIndex);
            int selectedRow = sourceIndex.row();
            saveRoomData(selectedRow);
        }
    }
    setEditMode(editingNow);
}


void RoomsWindow::on_CancelButton_clicked()
{
    ConfirmDialog dialog(this);
    dialog.changeText("Revert changes?");
    if (dialog.exec() == QDialog::Accepted) {
        QModelIndex proxyIndex = ui->roomsTableView->currentIndex();
        QModelIndex sourceIndex = proxyModel->mapToSource(proxyIndex);
        int selectedRow = sourceIndex.row();
        fillInRoomData(selectedRow);
        setEditMode(true);
    }
}