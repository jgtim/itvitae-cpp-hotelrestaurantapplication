#include "hotel/restaurantwindow/reservetable/reservetable.h"
#include "hotel/restaurantwindow/reservetable/ui_reservetable.h"
#include "hotel/restaurantwindow/restaurantwindow.h"
#include "readers/tablereservationserializer.h"
#include "hotel/restaurantwindow/selecttabledialog.h"

#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>


reservetable::reservetable(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::reservetable),
    db(QSqlDatabase::database())
{
    ui->setupUi(this);

    QRegularExpression nameRegex("^[A-Za-z\\s'-]{2,50}$");
    ui->nameEdit->setValidator(new QRegularExpressionValidator(nameRegex, this));

    ui->guestsEdit->setValidator(new QIntValidator(1, 6, this)); //maximum to max number of guests per table?

    int maxTableNumber = 50; // fallback if the parent/table list isn't available
    if (RestaurantWindow* parentWin = qobject_cast<RestaurantWindow *>(parent)) {
        if (QVector<Table>* tables = parentWin->getTables(); tables && !tables->isEmpty()) {
            maxTableNumber = tables->size();
        }
    }
    ui->tableEdit->setValidator(new QIntValidator(1, maxTableNumber, this));
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
    RestaurantWindow* parentWin = qobject_cast<RestaurantWindow *>(parent());
    if (!parentWin) {
        QMessageBox::warning(this, "Error", "Could not access restaurant data.");
        return;
    }

    QVector<Table>* tables = parentWin->getTables();
    if (!tables) {
        QMessageBox::warning(this, "Error", "No tables available.");
        return;
    }

    SelectTableDialog dialog(*tables, this);
    if (dialog.exec() == QDialog::Accepted) {
        int selectedTable = dialog.getSelectedTable();
        if (selectedTable > 0) {
            setTable(selectedTable);
        }
    }
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

    if (name.isEmpty() || name.length() < 2) {
        QMessageBox::warning(this, "Missing Information", "Please enter a valid guest name (at least 2 characters).");
        ui->nameEdit->setFocus();
        return;
    }

    bool ok = false;
    int guests = guestsStr.toInt(&ok);
    if (guestsStr.isEmpty() || !ok || guests < 1) {
        QMessageBox::warning(this, "Missing Information", "Please enter a valid number of guests (at least 1).");
        ui->guestsEdit->setFocus();
        return;
    }

    RestaurantWindow* parentWin = qobject_cast<RestaurantWindow *>(parent());
    if (!parentWin) {
        QDialog::accept();
        return;
    }

    QVector<Table>* tables = parentWin->getTables();
    QTableWidget* reservationTable = parentWin->getReservationTable();

    QDate selectedDate = getSelectedDate();
    TimeBlock selectedBlock = getSelectedTimeBlock();

    // Auto-assign table if field is left empty
    if (ui->tableEdit->text().trimmed().isEmpty() && tables) {
        for (Table &table : *tables) {
            bool canReserve = true;

            for (int r = 0; r < reservationTable->rowCount(); r++) {
                int tableNumber = reservationTable->item(r, 1) ? reservationTable->item(r, 1)->text().toInt() : -1;
                QDate resDate = reservationTable->item(r, 3) ? QDate::fromString(reservationTable->item(r, 3)->text(), "dd-MM-yyyy") : QDate();
                TimeBlock resBlock = reservationTable->item(r, 4) ? stringToTimeBlock(reservationTable->item(r, 4)->text()) : TimeBlock::Morning;

                if (table.getNumber() == tableNumber && resDate == selectedDate && resBlock == selectedBlock) {
                    canReserve = false;
                    break;
                }
            }

            if (canReserve && table.getStatus() != TableStatus::OCCUPIED) {
                ui->tableEdit->setText(QString::number(table.getNumber()));
                break;
            }
        }
    }

    // Validate that the table number actually exists and if it's Out Of Service
    int currentTable = ui->tableEdit->text().toInt();
    bool tableExists = false;
    bool tableIsOutOfService = false;
    if (tables) {
        for (const Table &table : *tables) {
            if (table.getNumber() == currentTable) {
                tableExists = true;
                if (table.getStatus() == TableStatus::OUT_OF_SERVICE){
                    tableIsOutOfService = true;
                }
                break;
            }
        }
    }
    if (!tableExists) {
        QMessageBox::warning(this, "Invalid Table", "There is no table with that number.");
        ui->tableEdit->setFocus();
        return;
    }
    if (tableIsOutOfService){
        QMessageBox::warning(this, "Reserving not possible", "This table is out of service.");
        ui->tableEdit->setFocus();
        return;
    }

    // Validate table availability
    for (int r = 0; r < reservationTable->rowCount(); r++) {
        int tableNumber = reservationTable->item(r, 1) ? reservationTable->item(r, 1)->text().toInt() : -1;
        QDate resDate = reservationTable->item(r, 3) ? QDate::fromString(reservationTable->item(r, 3)->text(), "dd-MM-yyyy") : QDate();
        TimeBlock resBlock = reservationTable->item(r, 4) ? stringToTimeBlock(reservationTable->item(r, 4)->text()) : TimeBlock::Morning;

        if (currentTable == tableNumber && resDate == selectedDate && resBlock == selectedBlock) {
            QMessageBox::warning(this, "Reserved", "This table is already reserved for this date and time block.");
            return;
        }
    }

    // Update status in the shared vector
    if (tables) {
        for (Table &table : *tables) {
            if (table.getNumber() == currentTable) {
                TableReservation res{.ReservationID = "unique ID",
                                     .guestName = name,
                                     .guestEmail = "email",
                                     .guestPhoneNumber = "phone number",
                                     .roomNumber = ui->roomNumberEdit->text(),
                                     .numberOfGuests = guests,
                                     .tableNumber = currentTable,
                                     .reservationDateTime = QDateTime(selectedDate, QTime()),
                                     .timeBlock = selectedBlock,
                                     .specialRequests = comments};
                saveTableToDatabase(res);
                table.setStatus(TableStatus::RESERVED);
                break;
            }
        }
    }

    QDialog::accept();
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
    QString dateStr = getSelectedDate().toString("dd-MM-yyyy");
    QString timeBlockStr = ui->timeBlockComboBox->currentText();
    QString comments = ui->commentsEdit->toPlainText();

    return {name, tablenumber, numguests, dateStr, timeBlockStr, comments};
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

void reservetable::saveTableToDatabase(TableReservation res){
    QSqlQuery tableQuery(db);
    tableQuery.prepare("INSERT INTO tableReservations values (:ReservationID, :GuestName, :GuestEmail, :GuestPhoneNumber, :RoomNumber, :NumberOfGuests, :TableNumber, :ReservationDateTime, :TimeBlock, :DurationMinutes, :Status, :SpecialRequests, :CreatedAt);");

    auto map = tableReservationSerializer::toVariantMap(res);

    tableQuery.bindValue(":ReservationID", map.value(QStringLiteral("reservation_id")).toString());
    tableQuery.bindValue(":GuestName", map.value(QStringLiteral("guest_name")).toString());
    tableQuery.bindValue(":GuestEmail", map.value(QStringLiteral("guest_email")).toString());
    tableQuery.bindValue(":GuestPhoneNumber", map.value(QStringLiteral("guest_phone")).toString());
    tableQuery.bindValue(":RoomNumber", map.value("room_number").toString());
    tableQuery.bindValue(":NumberOfGuests", map.value(QStringLiteral("number_of_guests")));
    tableQuery.bindValue(":TableNumber", map.value(QStringLiteral("table_number")));
    tableQuery.bindValue(":ReservationDateTime", map.value(QStringLiteral("reservation_datetime")).toString());
    tableQuery.bindValue(":TimeBlock", map.value(QStringLiteral("time_block")).toString());
    tableQuery.bindValue(":DurationMinutes", map.value(QStringLiteral("duration_minutes")).toString());
    tableQuery.bindValue(":Status", map.value(QStringLiteral("status")).toString());
    tableQuery.bindValue(":SpecialRequests", map.value(QStringLiteral("special_requests")).toString());
    tableQuery.bindValue(":CreatedAt", map.value(QStringLiteral("created_at")).toString());

    if (!tableQuery.exec()) {
        qDebug() << "Failed to insert reservation: " << tableQuery.lastError().text();
    }
}

QList<TableReservation> reservetable::getAllTableReservations() {
    QList<TableReservation> reservations;

    QSqlQuery resQuery("SELECT * FROM tableReservations;");

    int reservationIDIndex = resQuery.record().indexOf("ReservationID");
    int guestNameIndex = resQuery.record().indexOf("GuestName");
    int guestEmailIndex = resQuery.record().indexOf("GuestEmail");
    int guestPhoneNumberIndex = resQuery.record().indexOf("GuestPhoneNumber");
    int roomNumberIndex = resQuery.record().indexOf("RoomNumber");
    int numberOfGuestsIndex = resQuery.record().indexOf("NumberOfGuests");
    int tableNumberIndex = resQuery.record().indexOf("TableNumber");
    int reservationDateTimeIndex = resQuery.record().indexOf("ReservationDateTime");
    int durationMinutesIndex = resQuery.record().indexOf("DurationMinutes");
    int timeBlockIndex = resQuery.record().indexOf("TimeBlock");
    int statusIndex = resQuery.record().indexOf("Status");
    int specialRequestsIndex = resQuery.record().indexOf("SpecialRequests");
    int createdAtIndex = resQuery.record().indexOf("CreatedAt");

    while (resQuery.next()) {
        QVariantMap resMap;

        resMap[QStringLiteral("reservation_id")] = resQuery.value(reservationIDIndex).toString();
        resMap[QStringLiteral("guest_name")] = resQuery.value(guestNameIndex).toString();
        resMap[QStringLiteral("guest_email")] = resQuery.value(guestEmailIndex).toString();
        resMap[QStringLiteral("guest_phone")] = resQuery.value(guestPhoneNumberIndex).toString();
        resMap[QStringLiteral("room_number")] = resQuery.value(roomNumberIndex).toString();
        resMap[QStringLiteral("number_of_guests")] = resQuery.value(numberOfGuestsIndex).toString();
        resMap[QStringLiteral("table_number")] = resQuery.value(tableNumberIndex).toString();
        resMap[QStringLiteral("reservation_datetime")] = resQuery.value(reservationDateTimeIndex).toString();
        resMap[QStringLiteral("time_block")] = resQuery.value(timeBlockIndex).toString();
        resMap[QStringLiteral("duration_minutes")] = resQuery.value(durationMinutesIndex).toString();
        resMap[QStringLiteral("status")] = resQuery.value(statusIndex).toString();
        resMap[QStringLiteral("special_requests")] = resQuery.value(specialRequestsIndex).toString();
        resMap[QStringLiteral("created_at")] = resQuery.value(createdAtIndex).toString();

        reservations.append(tableReservationSerializer::fromVariantMap(resMap));
    }

    return reservations;
}
