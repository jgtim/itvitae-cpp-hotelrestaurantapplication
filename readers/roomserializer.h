#ifndef ROOMSERIALIZER_H
#define ROOMSERIALIZER_H

#include <QVariantMap>

#include "hotel/roomswindow/Room/room.h"

class RoomSerializer
{
public:
    // Serializes a Reservation struct into key/value map for SQLite
    static QVariantMap toVariantMap(const Room &room);

    // Deserializes a key/value map from SQLite into a Reservation struct
    static Room fromVariantMap(const QVariantMap &map);
};

#endif // ROOMSERIALIZER_H
