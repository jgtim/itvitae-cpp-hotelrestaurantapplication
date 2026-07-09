#include "roomstab.h"
#include "ui_roomstab.h"

RoomsTab::RoomsTab(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::RoomsTab)
{
    ui->setupUi(this);
}

RoomsTab::~RoomsTab()
{
    delete ui;
}

void RoomsTab::onDataChanged()
{
    qDebug() << "Rooms Tab received notification that data changed. Refreshing UI...";
}