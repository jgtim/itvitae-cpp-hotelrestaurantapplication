#ifndef RESERVATIONSERIALIZER_H
#define RESERVATIONSERIALIZER_H

#include <QVariantMap>

#include "hotel/reservationstab/Reservation/reservation.h"

class ReservationSerializer {
public:
    // Serializes a Reservation struct into key/value map for SQLite
    static QVariantMap toVariantMap(const Reservation &reservation);

    // Deserializes a key/value map from SQLite into a Reservation struct
    static Reservation fromVariantMap(const QVariantMap &map);
};

#endif // RESERVATIONSERIALIZER_H
