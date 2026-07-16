#include "roomdetailsdialog.h"
#include "ui_roomdetailsdialog.h"

RoomDetailsDialog::RoomDetailsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RoomDetailsDialog)
{
    ui->setupUi(this);

}

RoomDetailsDialog::~RoomDetailsDialog()
{
    delete ui;
}