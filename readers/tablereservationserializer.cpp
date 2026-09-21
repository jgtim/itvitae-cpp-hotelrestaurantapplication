#include "tablereservationserializer.h"

QVariantMap tableReservationSerializer::toVariantMap(const TableReservation &reservation) {
    QVariantMap map;

    map[QStringLiteral("reservation_id")]    = reservation.ReservationID;
    map[QStringLiteral("room_number")]        = reservation.roomNumber;
    map[QStringLiteral("guest_name")]        = reservation.guestName;
    map[QStringLiteral("guest_email")]       = reservation.guestEmail;
    map[QStringLiteral("guest_phone")]       = reservation.guestPhoneNumber;

    map[QStringLiteral("number_of_guests")] =  reservation.numberOfGuests;
    map[QStringLiteral("table_number")]       = reservation.tableNumber;
    map[QStringLiteral("reservation_datetime")] = reservation.reservationDateTime.toString(Qt::ISODate);
    map[QStringLiteral("time_block")] = timeBlockToString(reservation.timeBlock);
    map[QStringLiteral("duration_minutes")] = reservation.durationMinutes;

    map[QStringLiteral("status")] = reservationStatusToString(reservation.status);
    map[QStringLiteral("special_requests")] = reservation.specialRequests;
    map[QStringLiteral("created_at")] = reservation.createdAt.toString(Qt::ISODate);

    return map;
}

TableReservation tableReservationSerializer::fromVariantMap(const QVariantMap &map) {
    TableReservation reservation;

    reservation.ReservationID = map.value(QStringLiteral("reservation_id")).toString();
    reservation.roomNumber = map.value(QStringLiteral("room_number")).toString();
    reservation.guestName = map.value(QStringLiteral("guest_name")).toString();
    reservation.guestEmail = map.value(QStringLiteral("guest_email")).toString();
    reservation.guestPhoneNumber = map.value(QStringLiteral("guest_phone")).toString();
    reservation.numberOfGuests = map.value(QStringLiteral("number_of_guests")).toInt();
    reservation.tableNumber = map.value(QStringLiteral("table_number")).toInt();
    reservation.reservationDateTime = QDateTime::fromString(map.value(QStringLiteral("reservation_datetime")).toString(), Qt::ISODate);
    reservation.durationMinutes = map.value(QStringLiteral("duration_minutes")).toInt();
    reservation.timeBlock = stringToTimeBlock(map.value(QStringLiteral("time_block")).toString());
    reservation.status = stringToReservationStatus(map.value(QStringLiteral("status")).toString());
    reservation.specialRequests = map.value(QStringLiteral("special_requests")).toString();
    reservation.createdAt = QDateTime::fromString(map.value(QStringLiteral("created_at")).toString(), Qt::ISODate);

    return reservation;
}