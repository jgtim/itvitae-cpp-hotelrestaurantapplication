#ifndef ROOM_H
#define ROOM_H

#include <QMetaType>
#include <QList>
#include <QString>

// Colums from Excel Sheet: -------------------------------------------------------------
enum class RoomType {
    Economy,
    Standard,
    Luxurious,
    Unknown
};

RoomType stringToRoomType(const QString& str);

QString roomTypeToString(RoomType type);

// -------------------------------------------------------------------------------------

enum class RoomView {
    Standard,
    Mountain,
    Lake
};

RoomView stringToRoomView(const QString& str);

QString roomViewToString(RoomView view);

// Colums not in Excel sheet: -------------------------------------------------------
enum class RoomStatus {
    Available,
    Occupied,
    Reserved,
    Maintenance
};

RoomStatus stringToRoomStatus (const QString& str);

QString roomStatusToString(RoomStatus status);

// ----------------------------------------------------------------------------

enum class HouseKeepingStatus {
    Clean,
    Dirty
};

HouseKeepingStatus stringToHouseKeepingStatus (const QString& str);

QString houseKeepingStatusToString(HouseKeepingStatus status);

// ---------------------------------------------------------------------

struct Room {
    QString roomNumber;
    RoomType roomType;
    int adults;
    RoomView roomView;
    bool babyBed;
    QString beds;
    QString comments;
    double pricePerNight;

    RoomStatus roomStatus = RoomStatus::Available;
    HouseKeepingStatus houseKeepingStatus = HouseKeepingStatus::Clean;
};

// ----------------------------------------------------------------------------

void appendRoom(QList<Room>& roomList, const Room& room);

void appendRoom(QList<Room>& roomList,
                QString roomNumber,
                RoomType roomType,
                int adults,
                RoomView roomView,
                bool babyBed,
                const QString& beds,
                const QString& comments,
                double pricePerNight,
                RoomStatus status,
                HouseKeepingStatus housekeeping);

bool parseExcelToBool(const QString& str);

Q_DECLARE_METATYPE(Room)

#endif // ROOM_H
