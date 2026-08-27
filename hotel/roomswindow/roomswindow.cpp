#include "roomswindow.h"
#include "ui_roomswindow.h"
#include "roomdetailsdialog.h"
#include "confirmdialog.h"
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
    //ui->RoomTypeEdit->addItems(["A", "B"]);

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
        QStandardItem *statusItem = model->item(row, 3);
        statusItem->setForeground(QBrush(QColor(0, 0, 0)));


        if (statusItem)
        {
            QString status = statusItem->text();

            if (status == "Available")
                statusItem->setBackground(QColor(200, 255, 200));

            else if (status == "Occupied")
                statusItem->setBackground(QColor(255, 200, 200));

            else if (status == "Reserved")
                statusItem->setBackground(QColor(200, 220, 255));

            else if (status == "Out of Order")
                statusItem->setBackground(QColor(200, 200, 200));
        }


        // Housekeeping kolom = 9
        QStandardItem *housekeepingItem = model->item(row, 4);

        if (housekeepingItem)
        {
            QString housekeeping = housekeepingItem->text();
            housekeepingItem->setForeground(QBrush(QColor(0, 0, 0)));


            if (housekeeping == "Clean")
                housekeepingItem->setBackground(QColor(200, 255, 200));

            else if (housekeeping == "Needs Cleaning")
                housekeepingItem->setBackground(QColor(255, 200, 200));
        }
    }
}

void RoomsWindow::importExcelRooms(){
    std::optional<QList<RoomRecord>> roomList = parseRoomFile();

    if (roomList.has_value()) {
        for(int i = 0; i < roomList->size(); i++) {
            model->setItem(i, 0, new QStandardItem(roomList->at(i).roomNumber));
            model->setItem(i, 1, new QStandardItem(roomList->at(i).category));
            // turn int into string
            QString adultAmount = QString::number(roomList->at(i).adults);
            model->setItem(i, 2, new QStandardItem(adultAmount));
            model->setItem(i, 3, new QStandardItem(roomList->at(i).view));
            // turn bool into string
            QString babyBedString = (roomList->at(i).babyBedPossible) ? "Yes" : "No";
            model->setItem(i, 4, new QStandardItem(babyBedString));
            model->setItem(i, 5, new QStandardItem(roomList->at(i).bed));
            model->setItem(i, 6, new QStandardItem(roomList->at(i).comments));
            // turn double into string
            QString priceString = QString::number(roomList->at(i).pricePerNight);
            model->setItem(i, 7, new QStandardItem(QString("€ %1").arg(priceString)));
            // No value for these yet
            model->setItem(i, 8, new QStandardItem("Available"));
            model->setItem(i, 9, new QStandardItem("Clean"));
        }
    } else {
        qDebug() << "Could not import room data";
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
    if (dialog.exec() == QDialog::Accepted) {
        QList<QPair<QString, QString>> updatedDetails = dialog.getDetails();
        for (int col = 0; col < updatedDetails.size(); ++col) {
            QStandardItem *item = model->item(selectedRow, col);
            if (item) {
                item->setText(updatedDetails[col].second);
            }
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
    int columnCount = model->columnCount();

    for (int col = 0; col < columnCount; ++col) {
        QString header = model->horizontalHeaderItem(col)->text();
        QStandardItem *item = model->item(selectedRow, col);
        QString value = item ? item->text() : "";
        if(header == "Room Number"){
            ui->RoomNumEdit->setText(value);
        }else if(header == "Room type"){
            //ui->RoomTypeEdit->setText(value);
            //TODO set to right value
        }else if(header == "Capacity"){
            ui->CapacityEdit->setText(value);
        }else if(header == "Status"){
            ui->StatusEdit->setText(value);
        }else if(header == "Housekeeping"){
            ui->CleaningEdit->setText(value);
        }else if(header == "Comments"){
            ui->NotesEdit->setText(value);
        }
    }

    ui->roomStack->setCurrentIndex(1);
}


void RoomsWindow::on_ReturnButton_clicked()
{
    ConfirmDialog dialog(this);
    dialog.changeText("leave without saving changes?");
    if (dialog.exec() == QDialog::Rejected) {
        return;
    }
    ui->roomStack->setCurrentIndex(0);
}

void RoomsWindow::setEditMode(bool readMode){
    const QString newEditButtonText = (readMode) ? "Edit" : "Save";
    ui->EditButton->setText(newEditButtonText);
    ui->CancelButton->setVisible(!readMode);
    ui->RoomNumEdit->setReadOnly(readMode);
    ui->RoomTypeEdit->setEditable(readMode);
    ui->CapacityEdit->setReadOnly(readMode);
    ui->StatusEdit->setReadOnly(readMode);
    ui->CleaningEdit->setReadOnly(readMode);
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
        }
    }
    setEditMode(editingNow);
}


void RoomsWindow::on_CancelButton_clicked()
{
    ConfirmDialog dialog(this);
    dialog.changeText("revert changes?");
    if (dialog.exec() == QDialog::Accepted) {
        setEditMode(true);
    }
}

