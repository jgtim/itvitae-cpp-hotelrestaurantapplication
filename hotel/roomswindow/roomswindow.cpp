#include "roomswindow.h"
#include "ui_roomswindow.h"
#include "roomdetailsdialog.h"
#include "confirmdialog.h"
#include "Room/room.h"
#include "../../readers/hotelkamersreader.h"

#include <QStandardItemModel>
#include <QStandardItem>
#include <QColor>
#include <QBrush>
#include <QSortFilterProxyModel>
#include <QHeaderView>
#include <QMessageBox>



// #include <QSqlTableModel> -- Later voor SQL aanzetten

RoomsWindow::RoomsWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::RoomsWindow)
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
        "View     ",        //spaties om het vakje groter te maken
        "Babybed",
        "Bed type",
        "Comments        ", //spaties om het vakje groter te maken
        "Price per night",
        "Status",
        "Housekeeping",
        //"More Information" //for if we add button here later
    });


    importExcelRooms();
    proxyModel = new QSortFilterProxyModel(this);
    proxyModel->setSourceModel(model);
    proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    proxyModel->setFilterKeyColumn(-1);


    ui->roomsTableView->setModel(proxyModel);

    ui->roomsTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->roomsTableView->horizontalHeader()->setStretchLastSection(true);
    ui->roomsTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);

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

            if (housekeeping == "Clean")
                housekeepingItem->setBackground(QColor(200, 255, 200));
            else if (housekeeping == "Dirty")
                housekeepingItem->setBackground(QColor(255, 200, 200));
        }
    }
}

void RoomsWindow::importExcelRooms(){
    std::optional<QList<Room>> roomList = parseRoomFile();

    if (!roomList.has_value()) {
        qDebug() << "Could not import room data";
        QMessageBox::warning(this, "Import Error", "Failed to load room data from Excel.");
        return;
    }

    model->removeRows(0, model->rowCount()); // Clear any existing items

    for (int i = 0; i < roomList->size(); ++i) {
        const Room& room = roomList->at(i);

        // Column 0: Room Number
        model->setItem(i, 0, new QStandardItem(room.roomNumber));
        // Column 1: Room Type (Enum -> String)
        model->setItem(i, 1, new QStandardItem(roomTypeToString(room.roomType)));
        // Column 2: Adults
        model->setItem(i, 2, new QStandardItem(QString::number(room.adults)));
        // Column 3: Room View (Enum -> String)
        model->setItem(i, 3, new QStandardItem(roomViewToString(room.roomView)));
        // Column 4: Baby Bed (Bool -> String)
        model->setItem(i, 4, new QStandardItem(room.babyBed ? "Yes" : "No"));
        // Column 5: Bed Type
        model->setItem(i, 5, new QStandardItem(room.beds));
        // Column 6: Comments
        model->setItem(i, 6, new QStandardItem(room.comments));
        // Column 7: Price per Night (2 decimal places)
        QString priceString = QString("€ %1").arg(room.pricePerNight, 0, 'f', 2);
        model->setItem(i, 7, new QStandardItem(priceString));
        // Column 8: Room Status
        model->setItem(i, 8, new QStandardItem(roomStatusToString(room.roomStatus)));
        // Column 9: Housekeeping Status
        model->setItem(i, 9, new QStandardItem(houseKeepingStatusToString(room.houseKeepingStatus)));
    }
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
        }else if(header == "Comments        "){
            ui->NotesEdit->setText(value);
        }
    }
}

void RoomsWindow::on_seeRoomDetails_clicked()
{


    QModelIndex proxyIndex = ui->roomsTableView->currentIndex();

    if (!proxyIndex.isValid()) {
        QMessageBox::warning(this, "No Selection", "Please click on a reservation first to edit it.");
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
        dialog.changeText("leave without saving changes?");
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
        dialog.changeText("save changes?");
        if (dialog.exec() == QDialog::Rejected) {
            return;
        }else{
            //TODO save changes to database
        }
    }
    setEditMode(editingNow);
}


void RoomsWindow::on_CancelButton_clicked()
{
    ConfirmDialog dialog(this);
    dialog.changeText("revert changes?");
    if (dialog.exec() == QDialog::Accepted) {
        QModelIndex proxyIndex = ui->roomsTableView->currentIndex();
        QModelIndex sourceIndex = proxyModel->mapToSource(proxyIndex);
        int selectedRow = sourceIndex.row();
        fillInRoomData(selectedRow);
        setEditMode(true);
    }
}

