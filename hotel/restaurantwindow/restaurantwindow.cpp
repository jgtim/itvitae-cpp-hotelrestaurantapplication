#include "hotel/restaurantwindow/restaurantwindow.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QHeaderView>
#include <QRandomGenerator>
#include <QPainter>

#include <QMessageBox>
#include <cmath>

#include <iostream>
#include <qevent.h>
#include <qstandarditemmodel.h>

#include "reservetable/reservetable.h"

#include <QScrollArea>

RestaurantWindow::RestaurantWindow(QWidget *parent)
    : QWidget(parent), ui(nullptr)
{
    loadMockTables();
    setupLayout();
}

class TableCanvas : public QWidget
{Q_OBJECT
public:
    explicit TableCanvas(const QVector<Table>& tables, QWidget *parent = nullptr)
        : QWidget(parent), tables(tables)
    {

        updateCanvasSize();
    }

    void updateCanvasSize()
    {
        int maxX = 0;
        int maxY = 0;

        for (const Table& table : tables) {
            // Each table ellipse is 50x50 at (x, y)
            if (table.getX() + 50 > maxX) maxX = table.getX() + 50;
            if (table.getY() + 50 > maxY) maxY = table.getY() + 50;
        }
        // Add padding around the canvas edge
        setMinimumSize(maxX + 0, maxY + 0);
    }

    void updateTables(QVector<Table> newTables){
        tables = newTables;
    }

    int getSelectedTable(){
        return selectedTable;
    }




    QVector<Table>* getTables(){
        return &tables;
    }

protected:
    void paintEvent(QPaintEvent *event) override
    {
        Q_UNUSED(event);

        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);

        // =========================

        // =========================
        painter.setPen(QColor("#1E1E24"));
        painter.setFont(QFont("Arial", 16, QFont::Bold));
        painter.drawText(30, 30, "Inside");


        // =========================
        // Titel TERRAS
        // =========================
        painter.drawText(30, 245, "Terrace");


        // =========================
        // Scheidingslijn
        // =========================
        painter.setPen(QPen(QColor("#2D2D35"), 2));
        painter.drawLine(20, 225, width() - 20, 225);


        // =========================
        // Tafels tekenen
        // =========================
        for (const Table& table : tables) {
            paintTable(table, painter);
        }
    }

    void mousePressEvent(QMouseEvent *event) override
    {
        if (event->button() == Qt::LeftButton) {
            QPoint mousePos = event->pos();

            for (Table& table : tables) {
                // Determine circle center and radius based on your drawEllipse dimensions
                const int radius = 25;
                QPoint center(table.getX() + radius, table.getY() + radius);

                // Calculate distance from click point to circle center using Pythagorean theorem
                int dx = mousePos.x() - center.x();
                int dy = mousePos.y() - center.y();
                double distance = std::sqrt(dx * dx + dy * dy);

                // If click is within circle radius, trigger action
                if (distance <= radius) {
                    onTableClicked(table);
                    break; // Stop checking further tables once hit is found
                }
            }

        }

        QWidget::mousePressEvent(event);
    }

private:
    QVector<Table> tables;
    int selectedTable = 0;
    void onTableClicked(Table& table)
    {
        selectedTable = table.getNumber();

        update();
    }
    void paintTable(const Table& table, QPainter& painter){
        QColor fillColor;
        QColor textColor = QColor("#F5F5F5");

        if (table.getStatus() == TableStatus::FREE) {
            fillColor = QColor("#2ECC71");
            textColor = QColor("#1E1E24");
        }
        else if (table.getStatus() == TableStatus::OCCUPIED) {
            fillColor = QColor("#D9534F");
        }
        else if (table.getStatus() == TableStatus::RESERVED) {
            fillColor = QColor("#E69F00");
        }
        else if (table.getStatus() == TableStatus::OUT_OF_SERVICE) {
            fillColor = QColor("#808080");
        }

        painter.setBrush(fillColor);
        if(table.getNumber() == selectedTable){
            painter.setPen(QPen(QColor("#ECFA00"), 2));
        }else{
            painter.setPen(QPen(QColor("#2D2D35"), 2));
        }
        painter.drawEllipse(table.getX(), table.getY(), 50, 50);

        painter.setPen(textColor);
        painter.drawText(table.getX(), table.getY(), 50, 50,
                         Qt::AlignCenter,
                         QString::number(table.getNumber()));
    }

};

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

    // === Menukaart Header ===
    QLabel *menuLabel = new QLabel("Menu");
    menuLabel->setStyleSheet("font-weight: bold; font-size: 12px;");
    menuLabel->setAlignment(Qt::AlignCenter);
    menuLabel->setFixedHeight(40);
    leftLayout->addWidget(menuLabel);

    // === Reserveringen Tabel ===
    QLabel *reserveLabel = new QLabel("Reservations");
    reserveLabel->setStyleSheet("font-weight: bold; font-size: 12px;");
    leftLayout->addWidget(reserveLabel);

    reservationTable = new QTableWidget();
    reservationTable->setColumnCount(5);
    reservationTable->setHorizontalHeaderLabels(QStringList() << "Name" << "Table" << "Guests" << "Room" << "Time");
    // reservationTable->setMaximumHeight(120);
    reservationTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

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
    rightPanel->setStyleSheet("background-color: white;");

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


    reservationTableBtn = new QPushButton("Out of Service");
    reservationTableBtn->setStyleSheet("background-color: ##808080; color: #F5F5F5; padding: 10px; font-weight: bold; border-radius: 5px;");
    reservationTableBtn->setFixedHeight(40);
    connect(reservationTableBtn, &QPushButton::clicked, this, &RestaurantWindow::onReservationTableClicked);
    topButtonLayout->addWidget(reservationTableBtn);

    topButtonLayout->addStretch();
    rightLayout->addLayout(topButtonLayout);

    tableCanvas = new TableCanvas(tables);
    canvasPointer = qobject_cast<TableCanvas*>(tableCanvas);
    tableCanvas->setStyleSheet("background-color: white; border: 2px solid #2D2D35;");

    rightLayout->addWidget(tableCanvas);


    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea->setWidget(tableCanvas);
    scrollArea->setWidgetResizable(true); // Allows canvas to expand automatically when window expands
    tableCanvas->setStyleSheet("background-color: white; border: 2px solid #2D2D35;");

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
        reservationTable->setItem(i, 3, new QTableWidgetItem("Room XX"));  // placeholder
        reservationTable->setItem(i, 4, new QTableWidgetItem(res.reservationDateTime.toString("HH:mm")));
        reservationTable->setItem(i, 5, new QTableWidgetItem("→"));
    }
}

void RestaurantWindow::setupReservationTable()
{
    reservationTable->insertRow(0);
    reservationTable->setItem(0, 0, new QTableWidgetItem("Jansen"));
    reservationTable->setItem(0, 1, new QTableWidgetItem("6"));
    reservationTable->setItem(0, 2, new QTableWidgetItem("2"));
    reservationTable->setItem(0, 3, new QTableWidgetItem("5"));
    reservationTable->setItem(0, 4, new QTableWidgetItem("15:00"));


    reservationTable->insertRow(1);
    reservationTable->setItem(1, 0, new QTableWidgetItem("Smith"));
    reservationTable->setItem(1, 1, new QTableWidgetItem("4"));
    reservationTable->setItem(1, 2, new QTableWidgetItem("2"));
    reservationTable->setItem(1, 3, new QTableWidgetItem("7"));
    reservationTable->setItem(1, 4, new QTableWidgetItem("17:00"));


    reservationTable->insertRow(2);
    reservationTable->setItem(2, 0, new QTableWidgetItem("Janssen"));
    reservationTable->setItem(2, 1, new QTableWidgetItem("3"));
    reservationTable->setItem(2, 2, new QTableWidgetItem("2"));
    reservationTable->setItem(2, 3, new QTableWidgetItem("12"));
    reservationTable->setItem(2, 4, new QTableWidgetItem("18:00"));

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
    int selectedTable = canvasPointer->getSelectedTable();
    if(selectedTable > 0){

        for (Table &table : tables) {
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
    int selectedTable = canvasPointer->getSelectedTable();
    if(selectedTable > 0){
        tables[selectedTable - 1].setStatus(TableStatus::OCCUPIED);
        canvasPointer->updateTables(tables);
        canvasPointer->update();
    }else{
        QMessageBox::warning(this, "No Table Selected", "Please select a table first.");
    }

}

void RestaurantWindow::onReservationTableClicked()
{
    if(canvasPointer->getSelectedTable() > 0){
        reservetable dialog(this);
        int selectedTable = canvasPointer->getSelectedTable();
        if(selectedTable > 0){
            dialog.setTable(selectedTable);
        }
        if (dialog.exec() == QDialog::Accepted) {
            //TODO add to list of reservations. Not sure what to do about the comments or hotel room. might also be database later?
            int row = reservationTable->rowCount();
            reservationTable->insertRow(row);
            std::vector<QString> newstring = dialog.getdata();
            reservationTable->setItem(row, 0, new QTableWidgetItem(newstring[0])); //name
            reservationTable->setItem(row, 1, new QTableWidgetItem(newstring[1])); //table number
            reservationTable->setItem(row, 2, new QTableWidgetItem(newstring[2])); //number of guests
            reservationTable->setItem(row, 3, new QTableWidgetItem(newstring[3])); //room number pulled from nowhere
            reservationTable->setItem(row, 4, new QTableWidgetItem(newstring[4])); //time, get this from datetime & save date for the sake of detail screen
            reservationTable->setItem(row, 5, new QTableWidgetItem(newstring[5])); //time, get this from datetime & save date for the sake of detail screen

            canvasPointer->update();
        }
    }else{
        QMessageBox::warning(this, "No Table Selected", "Please select a table first.");
    }
}

#include "restaurantwindow.moc"
