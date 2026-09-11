#include "reservetable.h"
#include "hotel/restaurantwindow/reservetable/ui_reservetable.h"
//#include "ui_reservetable.h"
#include "../restaurantwindow.h"
#include <QMessageBox>
#include <iostream>
#include <QTime>


reservetable::reservetable(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::reservetable)
{
    ui->setupUi(this);
    ui->guestsEdit->setValidator(new QIntValidator(0, 100, this)); //maximum to max number of guests per table?
    ui->tableEdit->setValidator(new QIntValidator(0, 100, this)); //set maximum to highest table number if that can be pulled from somewhere
    ui->roomNumberEdit->setValidator(new QIntValidator(0, 100, this));
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
    //this function is empty now because everything was moved to accept()

}

void reservetable::accept()
{
    if (ui->nameEdit->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Missing Information", "Please enter a name.");
        ui->nameEdit->setFocus();
        return;
    }

    if (ui->guestsEdit->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Missing Information", "Please enter a number of guests.");
        ui->guestsEdit->setFocus();
        return;
    }

    QVector<Table>* tables = qobject_cast<RestaurantWindow *>(parent())->getTables();
    QTableWidget* reservationTable = qobject_cast<RestaurantWindow *>(parent())->getReservationTable();

    if (ui->tableEdit->text().trimmed().isEmpty()) {
        for (Table &table : *tables) {
            if(table.getStatus() == TableStatus::FREE){
                ui->tableEdit->setText(QString::number(table.getNumber()));
                break;
            }else{
                bool canReserve = true;
                for(int r = 0; r < reservationTable->model()->rowCount(); r++){
                    int tableNumber = stoi(reservationTable->item(r, 1)->text().toStdString());
                    QTime reservationTime = QTime::fromString(reservationTable->item(r, 3)->text(), "HH:mm");
                    if(table.getNumber() == tableNumber && reservationTime == ui->reserveTimeEdit->time()){
                        canReserve = false;
                        //TODO also set to false for some window before this time
                        break;
                    }
                }
                if(canReserve){
                    ui->tableEdit->setText(QString::number(table.getNumber()));
                    break;
                }
            }
        }
    }

    int currentTable = ui->tableEdit->text().toInt();
    for (Table &table : *tables) {
        if (table.getNumber() == currentTable) {
            for(int r = 0; r < reservationTable->model()->rowCount(); r++){
                int tableNumber = stoi(reservationTable->item(r, 1)->text().toStdString());
                QTime reservationTime = QTime::fromString(reservationTable->item(r, 3)->text(), "HH:mm");
                if(currentTable == tableNumber && reservationTime == ui->reserveTimeEdit->time()){
                    QMessageBox::warning(this, "reserved", "this table is already reserved at that time");
                    //TODO don't just do the exact time but also some range after it
                    //TODO table used here is just for current day, make sure to get table for the correct day
                    return;
                }
            }
            table.setStatus(TableStatus::RESERVED);
            QDialog::accept();
            return;
        }
    }
}


void reservetable::on_buttonBox_rejected()
{
    this->reject();
}

std::vector<QString> reservetable::getdata(){
    //get 4 in order: name, table number, room number, time
    QString name =  ui->nameEdit->text();
    QString tablenumber = ui->tableEdit->text();
    QString roomnumber = ui->roomNumberEdit->text();
    QString time = ui->reserveTimeEdit->time().toString("HH:mm");
    return {name, tablenumber, roomnumber, time};
}

void reservetable::setTable(int selectedTable){
    ui->tableEdit->setText(QString::number(selectedTable));
}

