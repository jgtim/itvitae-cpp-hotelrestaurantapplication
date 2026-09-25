#include "hotel/restaurantwindow/restaurantreservationsdetailsdialog.h"
#include "hotel/restaurantwindow/reservetable/reservetable.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>
#include <QLabel>
#include <QHeaderView>
#include <QDate>
#include <QMessageBox>
#include <QPushButton>

#include <QDebug>

RestaurantReservationsDetailsDialog::RestaurantReservationsDetailsDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("Reservation Details");
    setGeometry(100, 100, 1000, 600);
    setModal(false);

    allReservations = reservetable::getAllTableReservations();

    setupLayout();
    populateTables();
}

RestaurantReservationsDetailsDialog::~RestaurantReservationsDetailsDialog()
{
}

void RestaurantReservationsDetailsDialog::setupLayout()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    setupSearchBar();
    mainLayout->addLayout(searchLayout);

    QSplitter *splitter = new QSplitter(Qt::Horizontal);

    // Left panel
    QWidget *leftWidget = new QWidget();
    QVBoxLayout *leftLayout = new QVBoxLayout(leftWidget);
    QLabel *todayLabel = new QLabel("Today's Reservations");
    todayLabel->setStyleSheet("font-weight: bold; font-size: 12px;");
    leftLayout->addWidget(todayLabel);
    setupTodayTable();
    leftLayout->addWidget(todayTable);
    leftWidget->setLayout(leftLayout);
    connect(dateFilter, &QDateEdit::dateChanged, this, &RestaurantReservationsDetailsDialog::onDateFilterChanged);

    // Right panel
    QWidget *rightWidget = new QWidget();
    QVBoxLayout *rightLayout = new QVBoxLayout(rightWidget);
    QLabel *upcomingLabel = new QLabel("Upcoming Reservations");
    upcomingLabel->setStyleSheet("font-weight: bold; font-size: 12px;");
    rightLayout->addWidget(upcomingLabel);
    setupUpcomingTable();
    rightLayout->addWidget(upcomingTable);
    rightWidget->setLayout(rightLayout);

    splitter->addWidget(leftWidget);
    splitter->addWidget(rightWidget);
    splitter->setSizes(QList<int>() << 500 << 500);

    mainLayout->addWidget(splitter);
    setLayout(mainLayout);
}

void RestaurantReservationsDetailsDialog::setupSearchBar()
{
    searchLayout = new QHBoxLayout();

    QLabel *searchLabel = new QLabel("Search:");
    searchField = new QLineEdit();
    searchField->setPlaceholderText("Search on Name or Hotel Room...");

    QPushButton *resetButton = new QPushButton("Reset");

    QLabel *dateLabel = new QLabel("Date:");
    dateFilter = new QDateEdit();
    dateFilter->setDate(QDate::currentDate());
    dateFilter->setCalendarPopup(true);

    connect(dateFilter, &QDateEdit::dateChanged, this, &RestaurantReservationsDetailsDialog::onDateFilterChanged);
    connect(resetButton, &QPushButton::clicked, this, [this]() {
        searchField->clear();
        dateFilter->setDate(QDate::currentDate());
    });
    searchLayout->addWidget(searchLabel);
    searchLayout->addWidget(searchField, 1);
    searchLayout->addWidget(resetButton);
    searchLayout->addWidget(dateLabel);
    searchLayout->addWidget(dateFilter);
    searchLayout->addStretch();
}

void RestaurantReservationsDetailsDialog::setupTodayTable()
{
    todayTable = new QTableWidget();
    todayTable->setColumnCount(5);
    todayTable->setHorizontalHeaderLabels(QStringList() << "Name" << "Table Number" << "Time Slot" << "Room Number" << "Comments");
    connect(todayTable, &QTableWidget::doubleClicked, this, &RestaurantReservationsDetailsDialog::onTodayTableDoubleClicked);

    QHeaderView *header = todayTable->horizontalHeader();
    header->setSectionResizeMode(0, QHeaderView::Stretch);
    header->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    header->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    header->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    header->setSectionResizeMode(4, QHeaderView::Stretch);


    todayTable->verticalHeader()->setVisible(false);
    todayTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    todayTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    todayTable->setSelectionMode(QAbstractItemView::SingleSelection);
}

void RestaurantReservationsDetailsDialog::setupUpcomingTable()
{
    upcomingTable = new QTableWidget();
    upcomingTable->setColumnCount(6);
    upcomingTable->setHorizontalHeaderLabels(QStringList() << "Name" << "Table Number" << "Time Slot" << "Date" << "Room Number" << "Comments");
    connect(upcomingTable, &QTableWidget::doubleClicked, this, &RestaurantReservationsDetailsDialog::onUpcomingTableDoubleClicked);

    QHeaderView *header = upcomingTable->horizontalHeader();
    header->setSectionResizeMode(0, QHeaderView::Stretch);
    header->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    header->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    header->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    header->setSectionResizeMode(4, QHeaderView::ResizeToContents);
    header->setSectionResizeMode(5, QHeaderView::Stretch);

    upcomingTable->verticalHeader()->setVisible(false);
    upcomingTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    upcomingTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    upcomingTable->setSelectionMode(QAbstractItemView::SingleSelection);
}

void RestaurantReservationsDetailsDialog::populateTables()
{
    QDate today = QDate::currentDate();
    QDate filterDate = dateFilter->date();

    todayTable->setRowCount(0);
    upcomingTable->setRowCount(0);

    QVector<TableReservation> sortedReservations = allReservations;

    // Sort reservations by Date ascending, then TimeBlock ascending
    std::sort(sortedReservations.begin(), sortedReservations.end(),
              [](const TableReservation &a, const TableReservation &b) {
                  QDate dateA = a.reservationDateTime.date();
                  QDate dateB = b.reservationDateTime.date();

                  if (dateA != dateB) {
                      return dateA < dateB;
                  }
                  // If dates are identical, compare by timeBlock
                  return static_cast<int>(a.timeBlock) < static_cast<int>(b.timeBlock);
              }
              );

    for (const TableReservation &res : sortedReservations) {
        QDate resDate = res.reservationDateTime.date();

        if (isTimeBlockPassed(resDate, res.timeBlock)) {
            continue;
        }

        // Filter by search text
        QString searchText = searchField->text().toLower();
        if (!searchText.isEmpty()) {
            bool matchesSearch = res.guestName.toLower().contains(searchText) ||
                                 res.roomNumber.toLower().contains(searchText) ||
                                 QString::number(res.tableNumber).contains(searchText);
            if (!matchesSearch) {
                continue;
            }
        }

        if (resDate == today) {
            int row = todayTable->rowCount();
            todayTable->insertRow(row);
            todayTable->setItem(row, 0, new QTableWidgetItem(res.guestName));
            todayTable->setItem(row, 1, new QTableWidgetItem(QString::number(res.tableNumber)));
            todayTable->setItem(row, 2, new QTableWidgetItem(timeBlockToString(res.timeBlock)));
            todayTable->setItem(row, 3, new QTableWidgetItem(res.roomNumber));
            todayTable->setItem(row, 4, new QTableWidgetItem(res.specialRequests));

        } else if (resDate > today) {
            bool showReservation = (filterDate == today) ||
                                   (resDate == filterDate);

            if (showReservation) {
                int row = upcomingTable->rowCount();
                upcomingTable->insertRow(row);
                upcomingTable->setItem(row, 0, new QTableWidgetItem(res.guestName));
                upcomingTable->setItem(row, 1, new QTableWidgetItem(QString::number(res.tableNumber)));
                upcomingTable->setItem(row, 2, new QTableWidgetItem(timeBlockToString(res.timeBlock)));
                upcomingTable->setItem(row, 3, new QTableWidgetItem(res.reservationDateTime.toString("dd-MM-yyyy")));
                upcomingTable->setItem(row, 4, new QTableWidgetItem(res.roomNumber));
                upcomingTable->setItem(row, 5, new QTableWidgetItem(res.specialRequests));
            }
        }
    }
}

void RestaurantReservationsDetailsDialog::onSearchTextChanged(const QString &text)
{
    populateTables();
}

void RestaurantReservationsDetailsDialog::onDateFilterChanged()
{
    populateTables();
}

void RestaurantReservationsDetailsDialog::onTodayTableDoubleClicked()
{
    getTableDetails(todayTable);

}

void RestaurantReservationsDetailsDialog::onUpcomingTableDoubleClicked()
{
    getTableDetails(upcomingTable);
}

void RestaurantReservationsDetailsDialog::getTableDetails(QTableWidget* table){
    QMessageBox msgBox;
    msgBox.setWindowTitle("Notification");
    QString messagetext = "";
    msgBox.setIcon(QMessageBox::Information);
    int currentRow = table->currentRow();
    for (int col = 0; col < table->columnCount(); ++col) {
        messagetext += "<b>" + table->horizontalHeaderItem(col)->text() + "</b> ";
        messagetext += table->item(currentRow, col)->text() + "<br>";
    }
    msgBox.setText(messagetext);
    msgBox.exec();
}


