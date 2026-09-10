#include "reservetable.h"
#include "hotel/restaurantwindow/reservetable/ui_reservetable.h"
//#include "ui_reservetable.h"

reservetable::reservetable(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::reservetable)
{
    ui->setupUi(this);
    ui->guestsEdit->setValidator(new QIntValidator(0, 100, this)); //maximum to max number of guests per table?
    ui->tableEdit->setValidator(new QIntValidator(0, 100, this)); //set maximum to highest table number if that can be pulled from somewhere
}

reservetable::~reservetable()
{
    delete ui;
}

void reservetable::on_tableSelectButton_clicked()
{
    //TODO go to new pop-up that copies the same canvas from restaurantwindow.
}


void reservetable::on_buttonBox_accepted()
{
    //TODO check if answers are valid (table exists & is not already reserved at that time, number of guests not over table capacity)
    this->accept();

}


void reservetable::on_buttonBox_rejected()
{
    this->reject();
}

std::vector<QString> reservetable::getdata(){
    //get 4 in order: name, table number, room number, time
    QString name =  ui->nameEdit->text();
    QString tablenumber = ui->tableEdit->text();
    QString roomnumber = ""; // TODO add this maybe
    QString time = ui->reserveTimeEdit->time().toString("HH:mm");
    return {name, tablenumber, roomnumber, time};
}

void reservetable::setTable(int selectedTable){
    ui->tableEdit->setText(QString::number(selectedTable));
}

