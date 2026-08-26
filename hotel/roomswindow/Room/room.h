#ifndef ROOM_H
#define ROOM_H

#include <QMetaType>
#include <QList>
#include <QString>

// Colums from Excel Sheet: -------------------------------------------------------------
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

// Colums not in Excel sheet: -------------------------------------------------------
enum class RoomStatus {
    Available,
    Occupied,
    Reserved,
    Maintanance
};

enum class HouseKeepingStatus {
    Clean,
    Dirty
};

// ---------------------------------------------------------------------

struct Room {
    int roomNumber;
    RoomType roomType;
    int adults;
    RoomView roomView;
    bool babyBed;
    QString beds;
    QString comments;
    double pricePerNight;

    RoomStatus roomStatus;
    HouseKeepingStatus houseKeepingStatus;
};

Q_DECLARE_METATYPE(Room)

#endif // ROOM_H
