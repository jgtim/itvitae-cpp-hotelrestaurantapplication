#ifndef RESERVATION_H
#define RESERVATION_H

#include <QString>
#include <QDate>
#include "address.h"
#include "gender.h"

struct Reservation {
    QString ReservationID;

    QString GuestName;
    QString GuestPhoneNumber;
    Address address;
    QDate dateOfBirth;
    Gender gender;  // Has serialization and deserialization methods now

    int numberOfGuests;
    int roomNumber; // temporary int; change with Room objects later

    QDate checkIn;
    QDate checkOut;
};

// Allows Reservation to be stored inside a QVariant / QStandardItem
Q_DECLARE_METATYPE(Reservation)

#endif // RESERVATION_H
