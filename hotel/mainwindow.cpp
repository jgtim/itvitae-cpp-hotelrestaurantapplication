#include "hotel/mainwindow.h"
#include "hotel/ui_mainwindow.h"

#include "hotel/reservationstab/reservationstab.h"
#include "hotel/roomswindow//roomswindow.h"
#include "hotel/restaurantwindow/restaurantwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    tabWidget = new QTabWidget(this);
    setCentralWidget(tabWidget);

    // Add tabs here:
    reservationsTab = new ReservationsTab(this);
    roomswindow = new RoomsWindow(this);
    restaurantwindow = new RestaurantWindow(this);

    tabWidget->addTab(roomswindow, tr("Rooms"));
    tabWidget->addTab(reservationsTab, tr("Reservations"));
    tabWidget->addTab(restaurantwindow, tr("Restaurant"));

    connect(tabWidget, &QTabWidget::currentChanged, this, &MainWindow::onTabChanged);
    connect(reservationsTab, &ReservationsTab::dataChangedNotification, this, &MainWindow::handleDataChangedInMainWindow);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onTabChanged(int index)
{
    // Check if the newly selected tab is the Reservations Tab (index 0)
    if (index == 0) {
        reservationsTab->onDataChanged();
    }
}

void MainWindow::handleDataChangedInMainWindow()
{
    // qDebug() << "MainWindow was notified that reservation data was edited!";
}