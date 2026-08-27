#ifndef ROOM_H
#define ROOM_H

#include <QMetaType>

enum class RoomType {
    Economy,
    Standard,
    Luxurious
};

enum class RoomView {
    Standard,
    Mountain,
    Lake
};

struct Room {
    int roomNumber;
    RoomType roomType;
    int adults;
    RoomView roomView;
    bool babyBed;
    QString beds;
    QString comments;
    double pricePerNight;
};

Q_DECLARE_METATYPE(Room)

#endif // ROOM_H
