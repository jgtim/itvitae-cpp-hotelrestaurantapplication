#ifndef TABLERESERVATION_H
#define TABLERESERVATION_H

#include <QString>
#include <QDateTime>
#include <QMetaType>
#include <optional>

#include "hotel/reservationstab/Reservation/reservation.h"
#include "ReservationStatus.h"

struct TableReservation {
    QString ReservationID;

    // Guest details
    QString guestName;
    QString guestEmail;
    QString guestPhoneNumber;

    // Reservation details
    int numberOfGuests = 1;
    int tableNumber = 1; // Should be between 1 and 31
    QDateTime reservationDateTime;
    int durationMinutes = 120; // default seating length, ASKED MAVI! WHATS ANSWER?

    std::optional<Reservation> hotelReservation;

    // Extra info
    ReservationStatus status = ReservationStatus::Confirmed;
    QString specialRequests;
    QDateTime createdAt = QDateTime::currentDateTime();

    // Helper functions
    QDateTime endDateTime() const {
        return reservationDateTime.addSecs(durationMinutes * 60);
    }
};

Q_DECLARE_METATYPE(TableReservation);

#endif // TABLERESERVATION_H
