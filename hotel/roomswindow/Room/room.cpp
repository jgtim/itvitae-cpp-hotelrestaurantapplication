#include "hotel/roomswindow/Room/room.h"

RoomType stringToRoomType(const QString& str){
    QString lower = str.trimmed().toLower();
    if (lower == "economy") return RoomType::Economy;
    if (lower == "standard") return RoomType::Standard;
    if (lower == "luxurious") return RoomType::Luxurious;
    return RoomType::Unknown;
}

QString roomTypeToString(RoomType type) {
    switch (type) {
    case RoomType::Economy:   return "Economy";
    case RoomType::Standard:  return "Standard";
    case RoomType::Luxurious: return "Luxurious";
    default:                  return "Unknown";
    }
}

// -------------------------------------------------------------------------------------

RoomView stringToRoomView(const QString& str) {
    QString lower = str.trimmed().toLower();
    if (lower == "standard") return RoomView::Standard;
    if (lower == "mountain") return RoomView::Mountain;
    if (lower == "lake" || lower == "lake view") return RoomView::Lake;
    return RoomView::Standard;
}

QString roomViewToString(RoomView view) {
    switch (view) {
    case RoomView::Standard: return "Standard";
    case RoomView::Mountain: return "Mountain";
    case RoomView::Lake:     return "Lake";
    default:                 return "Standard";
    }
}

// -------------------------------------------------------------------------------------

RoomStatus stringToRoomStatus (const QString& str) {
    QString lower = str.trimmed().toLower();
    if (lower == "available") return RoomStatus::Available;
    if (lower == "occupied") return RoomStatus::Occupied;
    if (lower == "reserved") return RoomStatus::Reserved;
    if (lower == "maintenance") return RoomStatus::Maintenance;
    return RoomStatus::Available;
}

QString roomStatusToString(RoomStatus status) {
    switch (status) {
    case RoomStatus::Available:   return "Available";
    case RoomStatus::Occupied:    return "Occupied";
    case RoomStatus::Reserved:    return "Reserved";
    case RoomStatus::Maintenance: return "Maintenance";
    default:                      return "Available";
    }
}

// -------------------------------------------------------------------------------------

HouseKeepingStatus stringToHouseKeepingStatus (const QString& str) {
    QString lower = str.trimmed().toLower();
    if (lower == "clean") return HouseKeepingStatus::Clean;
    if (lower == "dirty") return HouseKeepingStatus::Dirty;
    return HouseKeepingStatus::Dirty;
}

QString houseKeepingStatusToString(HouseKeepingStatus status) {
    switch (status) {
    case HouseKeepingStatus::Clean: return "Clean";
    case HouseKeepingStatus::Dirty: return "Needs Cleaning";
    default:                        return "Needs Cleaning";
    }
}

// -------------------------------------------------------------------------------------

void appendRoom(QList<Room>& roomList, const Room& room) {
    roomList.append(room);
}

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
                HouseKeepingStatus housekeeping)
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

bool parseExcelToBool(const QString& str) {
    QString cleanStr = str.trimmed().toLower();

    if (cleanStr == "yes" || cleanStr == "true" || cleanStr == "y" || cleanStr == "1") {
        return true;
    }

    return false;
}