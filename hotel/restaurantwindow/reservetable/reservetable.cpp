#include "hotel/restaurantwindow/reservetable/reservetable.h"
#include "hotel/restaurantwindow/reservetable/ui_reservetable.h"
#include "hotel/restaurantwindow/restaurantwindow.h"

#include <QMessageBox>


reservetable::reservetable(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::reservetable)
{
    ui->setupUi(this);

    QRegularExpression nameRegex("^[A-Za-z\\s'-]{2,50}$");
    ui->nameEdit->setValidator(new QRegularExpressionValidator(nameRegex, this));

    ui->guestsEdit->setValidator(new QIntValidator(1, 6, this)); //maximum to max number of guests per table?

    ui->tableEdit->setValidator(new QIntValidator(1, 50, this)); //set maximum to highest table number if that can be pulled from somewhere
    ui->roomNumberEdit->setValidator(new QIntValidator(1, 100, this));

    ui->dateEdit->setDate(QDate::currentDate());
    ui->dateEdit->setMinimumDate(QDate::currentDate());

    ui->timeBlockComboBox->clear();
    ui->timeBlockComboBox->addItem(timeBlockToString(TimeBlock::Morning),   static_cast<int>(TimeBlock::Morning));
    ui->timeBlockComboBox->addItem(timeBlockToString(TimeBlock::Afternoon), static_cast<int>(TimeBlock::Afternoon));
    ui->timeBlockComboBox->addItem(timeBlockToString(TimeBlock::Evening),   static_cast<int>(TimeBlock::Evening));

    connect(ui->commentsEdit, &QTextEdit::textChanged, this, [this]() {
        if (ui->commentsEdit->toPlainText().length() > 100) {
            QString txt = ui->commentsEdit->toPlainText();
            txt.truncate(100);
            ui->commentsEdit->setPlainText(txt);
            QTextCursor cursor = ui->commentsEdit->textCursor();
            cursor.movePosition(QTextCursor::End);
            ui->commentsEdit->setTextCursor(cursor);
        }
    });
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

    QString name = ui->nameEdit->text().trimmed();
    QString guestsStr = ui->guestsEdit->text().trimmed();
    QString comments = ui->commentsEdit->toPlainText().trimmed();

    if (name.isEmpty()) {
        QMessageBox::warning(this, "Missing Information", "Please enter a guest name.");
        ui->nameEdit->setFocus();
        return;
    }

    if (name.length() < 2) {
        QMessageBox::warning(this, "Missing Information", "Name must be at least 2 characters long.");
        ui->nameEdit->setFocus();
        return;
    }

    bool ok;
    int guests = guestsStr.toInt(&ok);
    if (guestsStr.isEmpty() || !ok || guests < 1) {
        QMessageBox::warning(this, "Missing Information", "Please enter a valid number of guests (at least 1).");
        ui->guestsEdit->setFocus();
        return;
    }

    if (comments.length() > 100) {
        QMessageBox::warning(this, "Exceeded Maximum", "Comments cannot exceed 250 characters.");
        ui->commentsEdit->setFocus();
        return;
    }

    QVector<Table>* tables = qobject_cast<RestaurantWindow *>(parent())->getTables();
    QTableWidget* reservationTable = qobject_cast<RestaurantWindow *>(parent())->getReservationTable();

    QDate selectedDate = getSelectedDate();
    TimeBlock selectedBlock = getSelectedTimeBlock();

    if (ui->tableEdit->text().trimmed().isEmpty()) {
        for (Table &table : *tables) {
            if(table.getStatus() == TableStatus::FREE){
                ui->tableEdit->setText(QString::number(table.getNumber()));
                break;
            } else {
                bool canReserve = true;

                for (int r = 0; r < reservationTable->model()->rowCount(); r++) {
                    int tableNumber = stoi(reservationTable->item(r, 1)->text().toStdString());
                    QDate resDate = QDate::fromString(reservationTable->item(r, 4)->text(), "yyyy-MM-dd");
                    TimeBlock resBlock = stringToTimeBlock(reservationTable->item(r, 5)->text());

                    if (table.getNumber() == tableNumber && resDate == selectedDate && resBlock == selectedBlock) {
                        canReserve = false;
                        break;
                    }
                }

                if (canReserve){
                    ui->tableEdit->setText(QString::number(table.getNumber()));
                    break;
                }
            }
        }
    }

    int currentTable = ui->tableEdit->text().toInt();
    for (Table &table : *tables) {
        if (table.getNumber() == currentTable) {
            for (int r = 0; r < reservationTable->rowCount(); r++) {
                int tableNumber = reservationTable->item(r, 1)->text().toInt();
                QDate resDate = QDate::fromString(reservationTable->item(r, 4)->text(), "dd-MM-yyy");
                TimeBlock resBlock = stringToTimeBlock(reservationTable->item(r, 5)->text());

                if (currentTable == tableNumber && resDate == selectedDate && resBlock == selectedBlock) {
                    QMessageBox::warning(this, "Reserved", "This table is already reserved for this time block..");
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
    QString numguests = ui->guestsEdit->text();
    QString roomnumber = ui->roomNumberEdit->text();
    QString dateStr = getSelectedDate().toString("yyyy-MM-dd");
    QString timeBlockStr = ui->timeBlockComboBox->currentText();
    QString comments = ui->commentsEdit->toPlainText();

    return {name, tablenumber, numguests, roomnumber, dateStr, timeBlockStr, comments};
}

void reservetable::setTable(int selectedTable){
    ui->tableEdit->setText(QString::number(selectedTable));
}

TimeBlock reservetable::getSelectedTimeBlock() const {
    return static_cast<TimeBlock>(ui->timeBlockComboBox->currentData().toInt());
}

QDate reservetable::getSelectedDate() const {
    return ui->dateEdit->date();
}


