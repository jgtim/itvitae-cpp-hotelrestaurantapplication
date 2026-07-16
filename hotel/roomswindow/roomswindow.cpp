#include "roomswindow.h"
#include "ui_roomswindow.h"
#include "roomdetailsdialog.h"

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
        "View     ",        //spaties om het vakje groter te maken
        "Babybed",
        "Bed type",
        "Comments        ", //spaties om het vakje groter te maken
        "Price per night",
        "Status",
        "Housekeeping"
    });

    model->setItem(0, 0, new QStandardItem("1.01"));
    model->setItem(0, 1, new QStandardItem("Economy"));
    model->setItem(0, 2, new QStandardItem("2"));
    model->setItem(0, 3, new QStandardItem("Normal"));
    model->setItem(0, 4, new QStandardItem("Yes"));
    model->setItem(0, 5, new QStandardItem("Twin"));
    model->setItem(0, 6, new QStandardItem(""));
    model->setItem(0, 7, new QStandardItem("89.00"));
    model->setItem(0, 8, new QStandardItem("Available"));
    model->setItem(0, 9, new QStandardItem("Clean"));


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
        QStandardItem *statusItem = model->item(row, 8);
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
        QStandardItem *housekeepingItem = model->item(row, 9);

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

void RoomsWindow::on_roomsTableView_doubleClicked(const QModelIndex &proxyIndex)
{
    if (!proxyIndex.isValid())
        return;

    QModelIndex sourceIndex = proxyModel->mapToSource(proxyIndex);
    int selectedRow = sourceIndex.row();

    QString roomNum      = model->item(selectedRow, 0)->text();
    QString roomType     = model->item(selectedRow, 1)->text();
    QString adults       = model->item(selectedRow, 2)->text();
    QString price        = model->item(selectedRow, 7)->text();
    QString status       = model->item(selectedRow, 8)->text();
    QString housekeeping = model->item(selectedRow, 9)->text();

    RoomDetailsDialog dialog(this);

    dialog.exec();
}