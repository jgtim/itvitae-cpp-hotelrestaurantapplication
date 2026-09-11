#ifndef RESERVATIONSTATUS_H
#define RESERVATIONSTATUS_H

#include <QMetaType>

enum class ReservationStatus {
    Pending,
    Confirmed,
    Seated,
    Completed,
    Cancelled,
    NoShow
};

Q_DECLARE_METATYPE(ReservationStatus);

#endif // RESERVATIONSTATUS_H
