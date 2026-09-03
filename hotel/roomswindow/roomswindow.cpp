#include "roomswindow.h"
#include "ui_roomswindow.h"
#include "roomdetailsdialog.h"
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

    // --- later voor SQL aanzetten ---
    //QSqlTableModel *model = new QSqlTableModel(this);

    // model->setTable("Rooms");
    // model->select();

    model->setHorizontalHeaderLabels({
        "Room Number",
        "Room type",
        "Adults",
        "View",        //spaties om het vakje groter te maken
        "Babybed",
        "Bed type",
        "Comments", //spaties om het vakje groter te maken
        "Price per night",
        "Status",
        "Housekeeping"
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
    ui->roomsTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

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
            else if (status == "Maintenace")
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
    if (!proxyIndex.isValid())
        return;

    QModelIndex sourceIndex = proxyModel->mapToSource(proxyIndex);
    int selectedRow = sourceIndex.row();


    QList<QPair<QString, QString>> roomDetails;
    int columnCount = model->columnCount();

    for (int col = 0; col < columnCount; ++col) {
        QString header = model->horizontalHeaderItem(col)->text();

        QStandardItem *item = model->item(selectedRow, col);
        QString value = item ? item->text() : "";

        roomDetails.append(qMakePair(header, value));
    }

    RoomDetailsDialog dialog(this);
    dialog.setDetails(roomDetails);
    dialog.exec();
}
