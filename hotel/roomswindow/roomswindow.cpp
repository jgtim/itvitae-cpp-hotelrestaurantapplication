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
#include <QPushButton>
#include <QSignalMapper>

#include <iostream>

#include "delegate.h"



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
        "Capacity",
        // "View     ",        //spaties om het vakje groter te maken
        // "Babybed",
        // "Bed type",
        // "Comments        ", //spaties om het vakje groter te maken
        // "Price per night",
        "Status",
        "Housekeeping",
        //"More Information"
    });

    //QSignalMapper *signalMapper = new QSignalMapper(this);




    model->setItem(0, 0, new QStandardItem("101"));
    model->setItem(0, 1, new QStandardItem("Economy"));
    model->setItem(0, 2, new QStandardItem("2"));
    // model->setItem(0, 3, new QStandardItem("Normal"));
    // model->setItem(0, 4, new QStandardItem("Yes"));
    // model->setItem(0, 5, new QStandardItem("Twin"));
    // model->setItem(0, 6, new QStandardItem(""));
    // model->setItem(0, 7, new QStandardItem("89.00"));
    model->setItem(0, 3, new QStandardItem("Available"));
    model->setItem(0, 4, new QStandardItem("Clean"));
    //NOTE: room numbers also have to reflect this in updateRoomColors
    //model->setItem(0, 5, new QStandardItem("TEST"));

    //QPushButton* helpButton = new QPushButton("Help");

    // ui->roomsTableView->setIndexWidget(model->index(0, 5), helpButton);
    //helpButton->show();
    // signalMapper->setMapping(helpButton, 0);
    // connect(helpButton, SIGNAL(clicked(bool)), signalMapper, SLOT(map()));
    // //this part below goes after the for loop that iterates over rows
    //connect(signalMapper, SIGNAL(mapped(int)), this, SLOT(doSomething(int)));

    // //alternate ways of connecting that also don't do anything
    //connect(helpButton, SIGNAL(clicked(bool)), this, SLOT(doSomething(int 0)));
    //helpButton->clicked()->connect(doSomething(0));


    proxyModel = new QSortFilterProxyModel(this);
    proxyModel->setSourceModel(model);
    proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    proxyModel->setFilterKeyColumn(-1);


    ui->roomsTableView->setModel(proxyModel);

    ui->roomsTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->roomsTableView->horizontalHeader()->setStretchLastSection(true);
    ui->roomsTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);


    // MyDelegate delegate;
    // ui->roomsTableView->setItemDelegate(&delegate);



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

void RoomsWindow::doSomething(int row){
    std::cout << "I did something" << std::endl;
}
void RoomsWindow::on_seeRoomDetails_clicked()
{
    ui->roomStack->setCurrentIndex(1);
}

