#include "readers/roomserializer.h"

QVariantMap RoomSerializer::toVariantMap(const Room &room) {
    QVariantMap map;

    map[QStringLiteral("room_number")] = room.roomNumber;
    map[QStringLiteral("room_type")] = roomTypeToString(room.roomType);
    map[QStringLiteral("adults")] = room.adults;
    map[QStringLiteral("room_view")] = roomViewToString(room.roomView);
    map[QStringLiteral("baby_bed")] = (room.babyBed) ? "true" : "false";
    map[QStringLiteral("beds")] = room.beds;
    map[QStringLiteral("comments")] = room.comments;
    map[QStringLiteral("price_per_night")] = room.pricePerNight;

    map[QStringLiteral("room_status")] = roomStatusToString(room.roomStatus);
    map[QStringLiteral("housekeeping_status")] = houseKeepingStatusToString(room.houseKeepingStatus);

    return map;
}

Room RoomSerializer::fromVariantMap(const QVariantMap &map) {
    Room room;

    room.roomNumber = map.value(QStringLiteral("room_number")).toString();
    room.roomType = stringToRoomType(map.value(QStringLiteral("room_type")).toString());
    room.adults = map.value(QStringLiteral("adults")).toInt();
    room.roomView = stringToRoomView(map.value("room_view").toString());
    room.babyBed = (map.value(QStringLiteral("baby_bed")).toString() == "true") ? true : false;
    room.beds = map.value(QStringLiteral("beds")).toString();
    room.comments = map.value(QStringLiteral("comments")).toString();
    room.pricePerNight = map.value(QStringLiteral("price_per_night")).toDouble();

    room.roomStatus = stringToRoomStatus(map.value(QStringLiteral("room_status")).toString());
    room.houseKeepingStatus = stringToHouseKeepingStatus(map.value(QStringLiteral("housekeeping_status")).toString());

    return room;
}