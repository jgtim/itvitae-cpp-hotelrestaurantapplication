#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include "hometab.h"
#include "reservationstab.h"
#include "roomstab.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    tabWidget = new QTabWidget(this);
    setCentralWidget(tabWidget);

    // Add tabs here:
    homeTab = new HomeTab(this);
    reservationsTab = new ReservationsTab(this);
    roomsTab = new RoomsTab(this);

    tabWidget->addTab(homeTab, tr("Home"));
    tabWidget->addTab(reservationsTab, tr("Reservations"));
    tabWidget->addTab(roomsTab, tr("Rooms"));

    connect(homeTab, &HomeTab::dataChanged,
            reservationsTab, &ReservationsTab::onDataChanged);

    connect(homeTab, &HomeTab::dataChanged,
            roomsTab, &RoomsTab::onDataChanged);
}

MainWindow::~MainWindow()
{
    delete ui;
}
