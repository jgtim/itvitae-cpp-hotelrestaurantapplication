#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include "hometab.h"
#include "reservationstab.h"
#include "roomswindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    tabWidget = new QTabWidget(this);
    setCentralWidget(tabWidget);

    // Add tabs here:
    homeTab = new HomeTab(this);
    reservationsTab = new ReservationsTab(this);
    roomswindow = new RoomsWindow(this);

    tabWidget->addTab(homeTab, tr("Home"));
    tabWidget->addTab(reservationsTab, tr("Reservations"));
    tabWidget->addTab(roomswindow, tr("Rooms"));

    connect(homeTab, &HomeTab::dataChanged,
            reservationsTab, &ReservationsTab::onDataChanged);

}

MainWindow::~MainWindow()
{
    delete ui;
}
