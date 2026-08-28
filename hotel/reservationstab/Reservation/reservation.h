#ifndef RESERVATION_H
#define RESERVATION_H

#include <QString>
#include <QDate>
#include "../Guest/guest.h"
#include "PaymentStatus.h"

struct Reservation {
    QString ReservationID;

    Guest guest;

    int numberOfGuests;
    int roomNumber; // temporary int; change with Room objects later

    QDate checkIn;
    QDate checkOut;

    PaymentStatus paymentStatus;
};

// Allows Reservation to be stored inside a QVariant / QStandardItem
Q_DECLARE_METATYPE(Reservation)

#endif // RESERVATION_H
