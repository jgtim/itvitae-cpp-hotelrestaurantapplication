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

RoomType stringToRoomType(const QString& str) {
    QString lower = str.trimmed().toLower();
    if (lower == "economy") return RoomType::Economy;
    if (lower == "standard") return RoomType::Standard;
    if (lower == "luxurious") return RoomType::Luxurious;
    return RoomType::Unknown;
}

// -------------------------------------------------------------------------------------

enum class RoomView {
    Standard,
    Mountain,
    Lake
};

RoomView stringToRoomView(const QString& str) {
    QString lower = str.trimmed().toLower();
    if (lower == "standard") return RoomView::Standard;
    if (lower == "mountain") return RoomView::Mountain;
    if (lower == "lake") return RoomView::Lake;
    return RoomView::Standard;
}

// Colums not in Excel sheet: -------------------------------------------------------
enum class RoomStatus {
    Available,
    Occupied,
    Reserved,
    Maintanance
};

RoomStatus stringToRoomStatus (const QString& str) {
    QString lower = str.trimmed().toLower();
    if (lower == "available") return RoomStatus::Available;
    if (lower == "occupied") return RoomStatus::Occupied;
    if (lower == "reserved") return RoomStatus::Reserved;
    if (lower == "maintanance") return RoomStatus::Maintanance;
    return RoomStatus::Available;
}

// ----------------------------------------------------------------------------

enum class HouseKeepingStatus {
    Clean,
    Dirty
};

HouseKeepingStatus stringToHouseKeepingStatus (const QString& str) {
    QString lower = str.trimmed().toLower();
    if (lower == "clean") return HouseKeepingStatus::Clean;
    if (lower == "dirty") return HouseKeepingStatus::Dirty;
    return HouseKeepingStatus::Dirty;
}

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

    RoomStatus roomStatus = RoomStatus::Available;
    HouseKeepingStatus houseKeepingStatus = HouseKeepingStatus::Clean;
};

// ----------------------------------------------------------------------------

bool parseExcelToBool(const QString& str) {
    QString cleanStr = str.trimmed().toLower();

    if (cleanStr == "yes" || cleanStr == "true" || cleanStr == "y" || cleanStr == "1") {
        return true;
    }

    return false;
}

Q_DECLARE_METATYPE(Room)

#endif // ROOM_H
