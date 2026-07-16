#include "roomswindow.h"
#include "ui_roomswindow.h"

#include <QStandardItemModel>
#include <QStandardItem>
#include <QColor>
#include <QBrush>
#include <QSortFilterProxyModel>
#include <QHeaderView>

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
        "Floor",
        "Room type",
        "Capacity",
        "Status",
        "Housekeeping"
    });

    model->setItem(0, 0, new QStandardItem("1.01"));
    model->setItem(0, 1, new QStandardItem("1"));
    model->setItem(0, 2, new QStandardItem("Economy"));
    model->setItem(0, 3, new QStandardItem("2"));
    model->setItem(0, 4, new QStandardItem("Available"));
    model->setItem(0, 5, new QStandardItem("Clean"));

    model->setItem(1, 0, new QStandardItem("2.02"));
    model->setItem(1, 1, new QStandardItem("2"));
    model->setItem(1, 2, new QStandardItem("Luxurious"));
    model->setItem(1, 3, new QStandardItem("4"));
    model->setItem(1, 4, new QStandardItem("Occupied"));
    model->setItem(1, 5, new QStandardItem("Needs Cleaning"));


    proxyModel = new QSortFilterProxyModel(this);
    proxyModel->setSourceModel(model);
    proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    proxyModel->setFilterKeyColumn(-1);

    ui->roomsTableView->setModel(proxyModel);

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
        // Status kolom = 4
        QStandardItem *statusItem = model->item(row, 4);

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


        // Housekeeping kolom = 5
        QStandardItem *housekeepingItem = model->item(row, 5);

        if (housekeepingItem)
        {
            QString housekeeping = housekeepingItem->text();

            if (housekeeping == "Clean")
                housekeepingItem->setBackground(QColor(200, 255, 200));

            else if (housekeeping == "Needs Cleaning")
                housekeepingItem->setBackground(QColor(255, 200, 200));
        }
    }
}
