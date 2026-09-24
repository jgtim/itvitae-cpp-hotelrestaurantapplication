#include "hotel/restaurantwindow/restaurantwindow.h"
#include "hotel/restaurantwindow/reservetable/timeBlock.h"

#include "hotel/restaurantwindow/TableCanvas/tablecanvas.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QHeaderView>
#include <QRandomGenerator>
#include <QPainter>

#include <QMessageBox>
#include <cmath>

#include <qevent.h>
#include <qstandarditemmodel.h>

#include "hotel/restaurantwindow/reservetable/reservetable.h"

#include <QScrollArea>

#include <QDebug>

RestaurantWindow::RestaurantWindow(QWidget *parent)
    : QWidget(parent), ui(nullptr)
{
    //loadMockTables();
    setupLayout();
    tables = *(canvasPointer->getTables());
}

RestaurantWindow::~RestaurantWindow()
{
}

void RestaurantWindow::setupLayout()
{
    // Main layout
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // Create splitter (30% left, 70% right)
    mainSplitter = new QSplitter(Qt::Horizontal);

    // ===== LEFT PANEL (30%) =====
    setupLeftPanel();
    mainSplitter->addWidget(leftPanel);

    // ===== RIGHT PANEL (70%) =====
    setupRightPanel();
    mainSplitter->addWidget(rightPanel);

    // Set splitter sizes (45/55)
    mainSplitter->setSizes(QList<int>() << 450 << 550);
    mainSplitter->setStretchFactor(0, 1);
    mainSplitter->setStretchFactor(1, 1);

    mainLayout->addWidget(mainSplitter);
    setLayout(mainLayout);
}

void RestaurantWindow::setupLeftPanel()
{
    leftPanel = new QWidget();
    QVBoxLayout *leftLayout = new QVBoxLayout(leftPanel);
    leftLayout->setContentsMargins(10, 10, 10, 10);
    leftLayout->setSpacing(10);

    // // === Menukaart Header ===
    // QLabel *menuLabel = new QLabel("Menu");
    // menuLabel->setStyleSheet("font-weight: bold; font-size: 12px;");
    // menuLabel->setAlignment(Qt::AlignCenter);
    // menuLabel->setFixedHeight(40);
    // leftLayout->addWidget(menuLabel);

    // === Reserveringen Tabel ===
    QLabel *reserveLabel = new QLabel("Reservations");
    reserveLabel->setStyleSheet("font-weight: bold; font-size: 12px;");
    leftLayout->addWidget(reserveLabel);

    reservationTable = new QTableWidget();
    reservationTable->setColumnCount(5);
    reservationTable->setHorizontalHeaderLabels(QStringList() << "Name" << "Table" << "Guests" << "Date" << "Time Block");

    QHeaderView *header = reservationTable->horizontalHeader();
    header->setSectionResizeMode(0, QHeaderView::Stretch);
    header->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    header->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    header->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    header->setSectionResizeMode(4, QHeaderView::ResizeToContents);

    reservationTable->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    reservationTable->verticalHeader()->setDefaultSectionSize(35);

    reservationTable->setEditTriggers(QAbstractItemView::NoEditTriggers);   // Make table fields itself non-editable
    reservationTable->setSelectionBehavior(QAbstractItemView::SelectRows);  // Clicking any cell highlights whole row
    reservationTable->setSelectionMode(QAbstractItemView::SingleSelection); // User can only select one row at a time
    reservationTable->horizontalHeader()->setSectionsClickable(false);      // User cannot click a column header to highlight all rows beneath it
    reservationTable->verticalHeader()->setVisible(false);                  // Cannot see row numbers

    setupReservationTable();
    leftLayout->addWidget(reservationTable);

    // === Bekijk Reservering Details Button ===
    viewReservationBtn = new QPushButton("Reservation Details");
    viewReservationBtn->setFixedHeight(35);
    connect(viewReservationBtn, &QPushButton::clicked, this, &RestaurantWindow::onViewReservationClicked);
    leftLayout->addWidget(viewReservationBtn);

    leftPanel->setLayout(leftLayout);
}

void RestaurantWindow::setupRightPanel()
{
    rightPanel = new QWidget();
    rightPanel->setStyleSheet("background-color: #F8F9FA;");

    QVBoxLayout *rightLayout = new QVBoxLayout(rightPanel);
    rightLayout->setContentsMargins(10, 10, 10, 10);
    rightLayout->setSpacing(10);

    // === Top Button Bar ===
    QHBoxLayout *topButtonLayout = new QHBoxLayout();
    topButtonLayout->setSpacing(10);

    setFreeBtn = new QPushButton("Free");
    setFreeBtn->setStyleSheet("background-color: #2ECC71; color: #1E1E24; padding: 10px; font-weight: bold; border-radius: 5px;");
    setFreeBtn->setFixedHeight(40);
    connect(setFreeBtn, &QPushButton::clicked, this, &RestaurantWindow::onSetFreeClicked);
    topButtonLayout->addWidget(setFreeBtn);

    setOccupiedBtn = new QPushButton("Occupied");
    setOccupiedBtn->setStyleSheet("background-color: #D9534F; color: #F5F5F5; padding: 10px; font-weight: bold; border-radius: 5px;");
    setOccupiedBtn->setFixedHeight(40);
    connect(setOccupiedBtn, &QPushButton::clicked, this, &RestaurantWindow::onSetOccupiedClicked);
    topButtonLayout->addWidget(setOccupiedBtn);

    reservationTableBtn = new QPushButton("Reserved");
    reservationTableBtn->setStyleSheet("background-color: #E69F00; color: #F5F5F5; padding: 10px; font-weight: bold; border-radius: 5px;");
    reservationTableBtn->setFixedHeight(40);
    connect(reservationTableBtn, &QPushButton::clicked, this, &RestaurantWindow::onReservationTableClicked);
    topButtonLayout->addWidget(reservationTableBtn);

    outOfServiceBtn = new QPushButton("Out of Service");
    outOfServiceBtn->setStyleSheet("background-color: #808080; color: #F5F5F5; padding: 10px; font-weight: bold; border-radius: 5px;");
    outOfServiceBtn->setFixedHeight(40);
    connect(outOfServiceBtn, &QPushButton::clicked, this, &RestaurantWindow::onSetOutOfServiceClicked);
    topButtonLayout->addWidget(outOfServiceBtn);

    topButtonLayout->addStretch();
    rightLayout->addLayout(topButtonLayout);

    tableCanvas = new TableCanvas(tables);
    canvasPointer = qobject_cast<TableCanvas*>(tableCanvas);
    tableCanvas->setStyleSheet("background-color: #F8F9FA; border: 2px solid #2D2D35;");

    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea->setWidget(tableCanvas);
    scrollArea->setWidgetResizable(true); // Allows canvas to expand automatically when window expands
    tableCanvas->setStyleSheet("background-color: #F8F9FA; border: 2px solid #2D2D35;");

    rightLayout->addWidget(scrollArea);
    rightPanel->setLayout(rightLayout);
}

void RestaurantWindow::populateReservationTable(const QVector<TableReservation>& reservations)
{
    reservationTable->setRowCount(0);

    for (int i = 0; i < reservations.size(); ++i) {
        const TableReservation& res = reservations[i];

        reservationTable->insertRow(i);
        reservationTable->setItem(i, 0, new QTableWidgetItem(res.guestName));
        reservationTable->setItem(i, 1, new QTableWidgetItem(QString::number(res.tableNumber)));
        reservationTable->setItem(i, 2, new QTableWidgetItem(QString::number(res.numberOfGuests)));
        reservationTable->setItem(i, 3, new QTableWidgetItem(res.reservationDateTime.toString("dd-MM-yyyy")));
        reservationTable->setItem(i, 4, new QTableWidgetItem(timeBlockToString(res.timeBlock)));
    }
}

void RestaurantWindow::setupReservationTable()
{
    reservationTable->setRowCount(0); // Ensure table starts clean

    // auto addMockRow = [this](int row, const QString &name, const QString &table,
    //                          const QString &guests,
    //                          const QString &date, const QString &timeBlock)
    // {
    //     reservationTable->insertRow(row);
    //     reservationTable->setItem(row, 0, new QTableWidgetItem(name));
    //     reservationTable->setItem(row, 1, new QTableWidgetItem(table));
    //     reservationTable->setItem(row, 2, new QTableWidgetItem(guests));
    //     reservationTable->setItem(row, 3, new QTableWidgetItem(date));
    //     reservationTable->setItem(row, 4, new QTableWidgetItem(timeBlock));
    // };

    // addMockRow(0, "Jansen",  "6", "2", "2026-09-17", "Afternoon");
    // addMockRow(1, "Smith",   "4", "2", "2026-09-17", "Evening");
    // addMockRow(2, "Janssen", "3", "2", "2026-09-18", "Morning");

    auto reservations = reservetable::getAllTableReservations();
    populateReservationTable(reservations);
}

void RestaurantWindow::loadMockTables()
{
    tables.clear();

    const int columns = 5;
    const int itemSpacingX = 75;
    const int itemSpacingY = 55;

    // =========================
    // BINNEN - tafels 1 t/m 15
    // =========================
    for (int i = 1; i <= 15; ++i) {

        int index = i - 1;

        int x = 30 + (index % columns) * itemSpacingX;
        int y = 50 + (index / columns) * itemSpacingY;

        int randomStatus = QRandomGenerator::global()->bounded(3);

        TableStatus status;

        if (randomStatus == 0)
            status = TableStatus::FREE;
        else if (randomStatus == 1)
            status = TableStatus::OCCUPIED;
        else
            status = TableStatus::RESERVED;

        tables.append(Table(i, x, y, status));
    }


    // =========================
    // TERRAS - tafels 16 t/m 31
    // =========================
    for (int i = 16; i <= 31; ++i) {

        int index = i - 16;

        int x = 30 + (index % columns) * itemSpacingX;
        int y = 250 + (index / columns) * itemSpacingY;

        TableStatus status = TableStatus::OUT_OF_SERVICE;

        tables.append(Table(i, x, y, status));

        // int randomStatus = QRandomGenerator::global()->bounded(3);

        // TableStatus status;

        // if (randomStatus == 0)
        //     status = TableStatus::FREE;
        // else if (randomStatus == 1)
        //     status = TableStatus::OCCUPIED;
        // else
        //     status = TableStatus::RESERVED;

        // tables.append(Table(i, x, y, status));
    }
}

QVector<Table>* RestaurantWindow::getTables(){
    return this->canvasPointer->getTables();
}

QTableWidget* RestaurantWindow::getReservationTable(){
    return reservationTable;
}

// Slots
void RestaurantWindow::onDataChanged()
{
    // TODO: Implementatie
}

void RestaurantWindow::onViewReservationClicked()
{
    // TODO: Implementatie
}

void RestaurantWindow::onSetFreeClicked()
{
    qDebug() << "IT IS OCCUPIED";
    int selectedTable = canvasPointer->getSelectedTable();
    qDebug() << selectedTable;
    if(selectedTable > 0){

        for (Table &table : tables) {
            qDebug() << table.getNumber();
            if (table.getNumber() == selectedTable) {
                for(int r = 0; r < reservationTable->model()->rowCount(); r++){
                    int tableNumber = stoi(reservationTable->item(r, 1)->text().toStdString());
                    if(selectedTable == tableNumber){
                        table.setStatus(TableStatus::RESERVED);
                        canvasPointer->updateTables(tables);
                        canvasPointer->update();
                        break;
                    }
                }
                if(tables[selectedTable - 1].getStatus() == TableStatus::OCCUPIED){
                    table.setStatus(TableStatus::FREE);
                    canvasPointer->updateTables(tables);
                    canvasPointer->update();
                }
                break;
            }
        }

    }else{
        QMessageBox::warning(this, "No Table Selected", "Please select a table first.");
    }
}

void RestaurantWindow::onSetOccupiedClicked()
{
    qDebug() << "TEST";
    int selectedTable = canvasPointer->getSelectedTable();
    for (Table &table : tables) {
        qDebug() << table.getNumber();
    }
    if(selectedTable > 0){
        tables[selectedTable - 1].setStatus(TableStatus::OCCUPIED);
        canvasPointer->updateTables(tables);
        canvasPointer->update();
    }else{
        QMessageBox::warning(this, "No Table Selected", "Please select a table first.");
    }

}

void RestaurantWindow::onSetOutOfServiceClicked()
{
    int selectedTable = canvasPointer->getSelectedTable();

    if(selectedTable > 0){

        // Check if this table has a reservation
        for(int r = 0; r < reservationTable->model()->rowCount(); r++){
            int tableNumber = stoi(reservationTable->item(r, 1)->text().toStdString());

            if(selectedTable == tableNumber){
                QMessageBox::warning(
                    this,
                    "Table Reserved",
                    "This table is reserved, please move the reservation before completing this action"
                    );
                return;
            }
        }

        // Table is not reserved, ask for confirmation
        QMessageBox::StandardButton reply = QMessageBox::question(
            this,
            "Out of Service",
            "Are you sure this table is out of service?",
            QMessageBox::Yes | QMessageBox::No
            );

        if(reply == QMessageBox::Yes){
            tables[selectedTable - 1].setStatus(TableStatus::OUT_OF_SERVICE);
            canvasPointer->updateTables(tables);
            canvasPointer->update();
        }

    }else{
        QMessageBox::warning(
            this,
            "No Table Selected",
            "Please select a table first."
            );
    }
}

void RestaurantWindow::onReservationTableClicked()
{
    int selectedTable = canvasPointer ? canvasPointer->getSelectedTable() : 0;

    reservetable dialog(this);
    if (selectedTable > 0) {
        dialog.setTable(selectedTable);
    }

    if(selectedTable > 0 && tables[selectedTable - 1].getStatus() == TableStatus::OUT_OF_SERVICE){
        QMessageBox::warning(
            this,
            "Reserving not possible",
            "This table is out of service."
            );
    }
    else if (dialog.exec() == QDialog::Accepted) {
        std::vector<QString> data = dialog.getdata();
        // Index 0: Name, 1: Table, 2: Guests, 3: Date, 4: TimeBlock

        int row = reservationTable->rowCount();
        reservationTable->insertRow(row);
        reservationTable->setItem(row, 0, new QTableWidgetItem(data[0])); // Name
        reservationTable->setItem(row, 1, new QTableWidgetItem(data[1])); // Table
        reservationTable->setItem(row, 2, new QTableWidgetItem(data[2])); // Guests
        reservationTable->setItem(row, 3, new QTableWidgetItem(data[3])); // Date
        reservationTable->setItem(row, 4, new QTableWidgetItem(data[4])); // Time Block String

        // Sync local window copy to canvas and force canvas repaint
        if (canvasPointer && canvasPointer->getTables()) {
            tables = *(canvasPointer->getTables());
            canvasPointer->update();
        }
    }
}

#include "restaurantwindow.moc"
