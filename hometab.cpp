#include "hometab.h"
#include "ui_hometab.h"

HomeTab::HomeTab(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::HomeTab)
{
    ui->setupUi(this);
}

HomeTab::~HomeTab()
{
    delete ui;
}

void HomeTab::onDataChanged()
{
    qDebug() << "Home Tab received notification that data changed. Refreshing UI...";
}
