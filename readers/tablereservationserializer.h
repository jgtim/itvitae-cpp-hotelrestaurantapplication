#ifndef TABLERESERVATIONSERIALIZER_H
#define TABLERESERVATIONSERIALIZER_H

#include <QVariantMap>
#include "hotel/restaurantwindow/TableReservations/tableReservation.h"

class tableReservationSerializer
{
public:
    static QVariantMap toVariantMap(const TableReservation &reservation);
    static TableReservation fromVariantMap(const QVariantMap &map);
};

#endif // TABLERESERVATIONSERIALIZER_H
