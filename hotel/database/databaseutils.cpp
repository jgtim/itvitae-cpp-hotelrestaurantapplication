#include "databaseutils.h"
#include <QSqlDatabase>
#include <QSqlQuery>

void prepareDatabaseFile() {
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("model.db");
    db.open();
    if(!db.open()) {
        qDebug() << "Unable to open database!";
    }
    QSqlQuery query = QSqlQuery(db);
    query.exec("create table reservations(ReservationID INT, GuestName TEXT, GuestPhoneNumber TEXT, Address TEXT, DateOfBirth TEXT, Gender TEXT, NumberOfGuests INT, RoomNumber INT, CheckInDate TEXT, CheckOutDate TEXT);");
    // todo: add more tables for rooms, customers etc.
    db.close();
}