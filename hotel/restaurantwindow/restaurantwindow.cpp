#include "restaurantwindow.h"
#include "ui_restaurantwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QHeaderView>
#include <QRandomGenerator>
#include <QPainter>
#include <QMessageBox>

#include <iostream>
#include <qevent.h>

int selectedTable = 0;
class TableCanvas;
TableCanvas* canvasPointer = nullptr;

RestaurantWindow::RestaurantWindow(QWidget *parent)
    : QWidget(parent)
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

    }

    void updateTables(QVector<Table> newTables){
        tables = newTables;
    }

    void repaintTable(){
        std::cout << "REPAINTING" << std::endl;
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);

        paintTable(tables[selectedTable - 1], painter);
    }

protected:
    void paintEvent(QPaintEvent *event) override
    {
        Q_UNUSED(event);

        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);

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
    void onTableClicked(Table& table)
    {
        selectedTable = table.getNumber();

        update();
    }
    void paintTable(const Table& table, QPainter& painter){
        QColor fillColor;
        QColor textColor = QColor("#F5F5F5");

        if (table.getStatus() == TableStatus::FREE) {
            fillColor = QColor("#2ECC71"); // some sage green
            textColor = QColor("#1E1E24"); // better readability
        }
        else if (table.getStatus() == TableStatus::OCCUPIED)
            fillColor = QColor("#D9534F"); // a soft little crimson
        else
            fillColor = QColor("#E69F00"); // lovely warm amber

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

    // Set splitter sizes (30/70)
    mainSplitter->setSizes(QList<int>() << 300 << 700);
    mainSplitter->setStretchFactor(0, 3);
    mainSplitter->setStretchFactor(1, 7);

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
    reservationTable->setColumnCount(4);
    reservationTable->setHorizontalHeaderLabels(QStringList() << "Name" << "Table Number" << "Hotel Room" << "Time");
    reservationTable->setMaximumHeight(120);
    reservationTable->horizontalHeader()->setStretchLastSection(true);
    setupReservationTable();
    leftLayout->addWidget(reservationTable);

    // === Bekijk Reservering Details Button ===
    viewReservationBtn = new QPushButton("Reservation Details");
    viewReservationBtn->setFixedHeight(35);
    connect(viewReservationBtn, &QPushButton::clicked, this, &RestaurantWindow::onViewReservationClicked);
    leftLayout->addWidget(viewReservationBtn);

    // === Bestellingen Tabel ===
    QLabel *orderLabel = new QLabel("Orders");
    orderLabel->setStyleSheet("font-weight: bold; font-size: 12px;");
    leftLayout->addWidget(orderLabel);

    orderTable = new QTableWidget();
    orderTable->setColumnCount(5);
    orderTable->setHorizontalHeaderLabels(QStringList() << "Status" << "Rooms Number" << "Menu Item(s)" << "Placed at" << "Tables");
    orderTable->setMaximumHeight(120);
    orderTable->horizontalHeader()->setStretchLastSection(true);
    setupOrderTable();
    leftLayout->addWidget(orderTable);

    // === Spacer (push buttons down) ===
    leftLayout->addStretch();

    // === Bottom Buttons ===
    QHBoxLayout *bottomButtonLayout = new QHBoxLayout();
    bottomButtonLayout->setSpacing(10);

    allOrdersBtn = new QPushButton("All Orders");
    allOrdersBtn->setFixedHeight(35);
    connect(allOrdersBtn, &QPushButton::clicked, this, &RestaurantWindow::onAllOrdersClicked);
    bottomButtonLayout->addWidget(allOrdersBtn);

    newOrderBtn = new QPushButton("New Order");
    newOrderBtn->setFixedHeight(35);
    connect(newOrderBtn, &QPushButton::clicked, this, &RestaurantWindow::onNewOrderClicked);
    bottomButtonLayout->addWidget(newOrderBtn);

    leftLayout->addLayout(bottomButtonLayout);

    leftPanel->setLayout(leftLayout);
}

void RestaurantWindow::setupRightPanel()
{
    rightPanel = new QWidget();
    rightPanel->setStyleSheet("background-color: #1E1E24;");

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

    setOccupiedBtn = new QPushButton("Occupy");
    setOccupiedBtn->setStyleSheet("background-color: #D9534F; color: #F5F5F5; padding: 10px; font-weight: bold; border-radius: 5px;");
    setOccupiedBtn->setFixedHeight(40);
    connect(setOccupiedBtn, &QPushButton::clicked, this, &RestaurantWindow::onSetOccupiedClicked);
    topButtonLayout->addWidget(setOccupiedBtn);

    reservationTableBtn = new QPushButton("Reserve");
    reservationTableBtn->setStyleSheet("background-color: #E69F00; color: #F5F5F5; padding: 10px; font-weight: bold; border-radius: 5px;");
    reservationTableBtn->setFixedHeight(40);
    connect(reservationTableBtn, &QPushButton::clicked, this, &RestaurantWindow::onReservationTableClicked);
    topButtonLayout->addWidget(reservationTableBtn);

    topButtonLayout->addStretch();
    rightLayout->addLayout(topButtonLayout);

    // === Canvas Area (zwart) ===
    tableCanvas = new TableCanvas(tables);
    canvasPointer = qobject_cast<TableCanvas*>(tableCanvas);
    tableCanvas->setStyleSheet("background-color: #1E1E24; border: 2px solid #2D2D35;");

    rightLayout->addWidget(tableCanvas);

    rightPanel->setLayout(rightLayout);
}

void RestaurantWindow::setupReservationTable()
{
    reservationTable->insertRow(0);
    reservationTable->setItem(0, 0, new QTableWidgetItem("Jansen"));
    reservationTable->setItem(0, 1, new QTableWidgetItem("6"));
    reservationTable->setItem(0, 2, new QTableWidgetItem("5"));
    reservationTable->setItem(0, 3, new QTableWidgetItem("15:00"));

    reservationTable->insertRow(1);
    reservationTable->setItem(1, 0, new QTableWidgetItem("Smith"));
    reservationTable->setItem(1, 1, new QTableWidgetItem("4"));
    reservationTable->setItem(1, 2, new QTableWidgetItem("7"));
    reservationTable->setItem(1, 3, new QTableWidgetItem("17:00"));

    reservationTable->insertRow(2);
    reservationTable->setItem(2, 0, new QTableWidgetItem("Janssen"));
    reservationTable->setItem(2, 1, new QTableWidgetItem("3"));
    reservationTable->setItem(2, 2, new QTableWidgetItem("12"));
    reservationTable->setItem(2, 3, new QTableWidgetItem("18:00"));
}

void RestaurantWindow::setupOrderTable()
{
    orderTable->insertRow(0);
    orderTable->setItem(0, 0, new QTableWidgetItem("Wordt bezorgd"));
    orderTable->setItem(0, 1, new QTableWidgetItem("5"));
    orderTable->setItem(0, 2, new QTableWidgetItem("Tosti × 2"));
    orderTable->setItem(0, 3, new QTableWidgetItem("15:42"));
    orderTable->setItem(0, 4, new QTableWidgetItem("Room Service"));

    orderTable->insertRow(1);
    orderTable->setItem(1, 0, new QTableWidgetItem("Klaar voor bezorging"));
    orderTable->setItem(1, 1, new QTableWidgetItem("12"));
    orderTable->setItem(1, 2, new QTableWidgetItem("Spaghetti Bolognese"));
    orderTable->setItem(1, 3, new QTableWidgetItem("16:03"));
    orderTable->setItem(1, 4, new QTableWidgetItem("Room Service"));

    orderTable->insertRow(2);
    orderTable->setItem(2, 0, new QTableWidgetItem("Wordt bereid"));
    orderTable->setItem(2, 1, new QTableWidgetItem("3"));
    orderTable->setItem(2, 2, new QTableWidgetItem("Gegrilleerde zalm + 1"));
    orderTable->setItem(2, 3, new QTableWidgetItem("16:05"));
    orderTable->setItem(2, 4, new QTableWidgetItem("7"));

    orderTable->insertRow(3);
    orderTable->setItem(3, 0, new QTableWidgetItem("Nog niet begonnen"));
    orderTable->setItem(3, 1, new QTableWidgetItem("6"));
    orderTable->setItem(3, 2, new QTableWidgetItem("Zoete aardappels"));
    orderTable->setItem(3, 3, new QTableWidgetItem("16:12"));
    orderTable->setItem(3, 4, new QTableWidgetItem("1"));
}

void RestaurantWindow::loadMockTables()
{
    tables.clear();

    for (int i = 1; i <= 31; ++i) {
        int x = 15 + ((i - 1) % 8) * 65;
        int y = 15 + ((i - 1) / 8) * 65;

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

void RestaurantWindow::onAllOrdersClicked()
{
    // TODO: Implementatie
}

void RestaurantWindow::onNewOrderClicked()
{
    // TODO: Implementatie
}

void RestaurantWindow::onSetFreeClicked()
{
    if(selectedTable > 0){
        //TODO: check if there's a reservation on this table, if so set it to reserved instead of free
        tables[selectedTable - 1].setStatus(TableStatus::FREE);
        canvasPointer->updateTables(tables);
        canvasPointer->update();
    }else{
        QMessageBox::warning(this, "No Table Selected", "Please select a table first.");
    }
}

void RestaurantWindow::onSetOccupiedClicked()
{
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
    if(selectedTable > 0){
        // TODO: Implementatie
        std::cout << "TESTING" << std::endl;
    }else{
        QMessageBox::warning(this, "No Table Selected", "Please select a table first.");
    }
}

#include "restaurantwindow.moc"