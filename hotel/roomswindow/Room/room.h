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

inline RoomType stringToRoomType(const QString& str) {
    QString lower = str.trimmed().toLower();
    if (lower == "economy") return RoomType::Economy;
    if (lower == "standard") return RoomType::Standard;
    if (lower == "luxurious") return RoomType::Luxurious;
    return RoomType::Unknown;
}

inline QString roomTypeToString(RoomType type) {
    switch (type) {
    case RoomType::Economy:   return "Economy";
    case RoomType::Standard:  return "Standard";
    case RoomType::Luxurious: return "Luxurious";
    default:                  return "Unknown";
    }
}

// -------------------------------------------------------------------------------------

enum class RoomView {
    Standard,
    Mountain,
    Lake
};

inline RoomView stringToRoomView(const QString& str) {
    QString lower = str.trimmed().toLower();
    if (lower == "standard") return RoomView::Standard;
    if (lower == "mountain") return RoomView::Mountain;
    if (lower == "lake") return RoomView::Lake;
    return RoomView::Standard;
}

inline QString roomViewToString(RoomView view) {
    switch (view) {
    case RoomView::Standard: return "Standard";
    case RoomView::Mountain: return "Mountain";
    case RoomView::Lake:     return "Lake";
    default:                 return "Standard";
    }
}

// Colums not in Excel sheet: -------------------------------------------------------
enum class RoomStatus {
    Available,
    Occupied,
    Reserved,
    Maintanance
};

inline RoomStatus stringToRoomStatus (const QString& str) {
    QString lower = str.trimmed().toLower();
    if (lower == "available") return RoomStatus::Available;
    if (lower == "occupied") return RoomStatus::Occupied;
    if (lower == "reserved") return RoomStatus::Reserved;
    if (lower == "maintanance") return RoomStatus::Maintanance;
    return RoomStatus::Available;
}

inline QString roomStatusToString(RoomStatus status) {
    switch (status) {
    case RoomStatus::Available:   return "Available";
    case RoomStatus::Occupied:    return "Occupied";
    case RoomStatus::Reserved:    return "Reserved";
    case RoomStatus::Maintanance: return "Maintanance";
    default:                      return "Available";
    }
}

// ----------------------------------------------------------------------------

enum class HouseKeepingStatus {
    Clean,
    Dirty
};

inline HouseKeepingStatus stringToHouseKeepingStatus (const QString& str) {
    QString lower = str.trimmed().toLower();
    if (lower == "clean") return HouseKeepingStatus::Clean;
    if (lower == "dirty") return HouseKeepingStatus::Dirty;
    return HouseKeepingStatus::Dirty;
}

inline QString houseKeepingStatusToString(HouseKeepingStatus status) {
    switch (status) {
    case HouseKeepingStatus::Clean: return "Clean";
    case HouseKeepingStatus::Dirty: return "Needs Cleaning";
    default:                        return "Needs Cleaning";
    }
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

inline void appendRoom(QList<Room>& roomList, const Room& room) {
    roomList.append(room);
}

inline void appendRoom(QList<Room>& roomList,
                       int roomNumber,
                       RoomType roomType,
                       int adults,
                       RoomView roomView,
                       bool babyBed,
                       const QString& beds,
                       const QString& comments,
                       double pricePerNight,
                       RoomStatus status = RoomStatus::Available,
                       HouseKeepingStatus housekeeping = HouseKeepingStatus::Clean)
{
    Room r;
    r.roomNumber = roomNumber;
    r.roomType = roomType;
    r.adults = adults;
    r.roomView = roomView;
    r.babyBed = babyBed;
    r.beds = beds;
    r.comments = comments;
    r.pricePerNight = pricePerNight;
    r.roomStatus = status;
    r.houseKeepingStatus = housekeeping;

    roomList.append(r);
}

inline bool parseExcelToBool(const QString& str) {
    QString cleanStr = str.trimmed().toLower();

    if (cleanStr == "yes" || cleanStr == "true" || cleanStr == "y" || cleanStr == "1") {
        return true;
    }

    return false;
}

Q_DECLARE_METATYPE(Room)

#endif // ROOM_H
