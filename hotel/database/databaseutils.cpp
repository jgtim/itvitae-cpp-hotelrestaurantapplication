#include "hotel/database/databaseutils.h"
#include <QSqlDatabase>
#include <QSqlQuery>

void prepareDatabaseFile() {
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("model.db");
    db.open();
    if(!db.open()) {
        qDebug() << "Unable to open database!";
    } else {
        qDebug() << "Database opened successfully!";
    }
    QSqlQuery query = QSqlQuery(db);
    query.exec("create table reservations(ReservationID INT, GuestID TEXT, NumberOfGuests INT, RoomNumber INT, CheckInDate TEXT, CheckOutDate TEXT, PaymentStatus TEXT);");
    query.exec("create table guests(GuestID TEXT, GuestName TEXT, GuestPhoneNumber TEXT, GuestEmail TEXT, GuestAddress TEXT, GuestDateOfBirth TEXT, GuestGender TEXT, GuestPaymentMethod TEXT, GuestIdType TEXT);");
    query.exec("create table rooms(RoomNumber TEXT, RoomType TEXT, Adults INT, RoomView TEXT, BabyBed TEXT, Beds TEXT, Comments TEXT, PricePerNight TEXT, RoomStatus TEXT, HousekeepingStatus TEXT);");

    QSqlDatabase::removeDatabase("QSQLITE");
    db.close();
}