#ifndef RESERVATION_H
#define RESERVATION_H

#include <QString>
#include <QDate>

#include "hotel/reservationstab/Guest/guest.h"
#include "hotel/reservationstab/Reservation/PaymentStatus.h"

struct Reservation {
    int rowid;
    QString ReservationID;

    Guest guest;

    int numberOfGuests;
    QString roomNumber; // In the Room struct, the roonNumber is also a QString. We can use a map for quick lookups

    QDate checkIn;
    QDate checkOut;

    PaymentStatus paymentStatus;
};

// Allows Reservation to be stored inside a QVariant / QStandardItem
Q_DECLARE_METATYPE(Reservation)

#endif // RESERVATION_H
