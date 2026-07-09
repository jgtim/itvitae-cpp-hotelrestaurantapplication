#include "reservationstab.h"
#include "ui_reservationstab.h"

ReservationsTab::ReservationsTab(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ReservationsTab)
{
    ui->setupUi(this);
}

ReservationsTab::~ReservationsTab()
{
    delete ui;
}

void ReservationsTab::onDataChanged()
{
    qDebug() << "Reservations Tab received notification that data changed. Refreshing UI...";
}