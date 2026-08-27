#include "reservationserializer.h"
#include "../hotel/reservationstab/Reservation/gender.h"

QVariantMap ReservationSerializer::toVariantMap(const Reservation &res) {
    QVariantMap map;

    // Can use the String data field directly
    map[QStringLiteral("reservation_id")]    = res.ReservationID;
    map[QStringLiteral("guest_name")]        = res.GuestName;
    map[QStringLiteral("guest_phone")]       = res.GuestPhoneNumber;

    // Putting the QDate datat type in a ISO-8601 for SQLite
    map[QStringLiteral("date_of_birth")]     = res.dateOfBirth.toString(Qt::ISODate);
    map[QStringLiteral("check_in")]          = res.checkIn.toString(Qt::ISODate);
    map[QStringLiteral("check_out")]         = res.checkOut.toString(Qt::ISODate);

    // Converting Gender enum class to String
    map[QStringLiteral("gender")]            = genderToString(res.gender);

    // Putting in the Integers directly
    map[QStringLiteral("number_of_guests")]  = res.numberOfGuests;
    map[QStringLiteral("room_number")]       = res.roomNumber;

    // Still need to do the Address struct

    return map;
}

Reservation ReservationSerializer::fromVariantMap(const QVariantMap &map) {
    Reservation res;

    res.ReservationID     = map.value(QStringLiteral("reservation_id")).toString();
    res.GuestName         = map.value(QStringLiteral("guest_name")).toString();
    res.GuestPhoneNumber  = map.value(QStringLiteral("guest_phone")).toString();

    // ISO-8601 to QDate
    res.dateOfBirth       = QDate::fromString(map.value(QStringLiteral("date_of_birth")).toString(), Qt::ISODate);
    res.checkIn           = QDate::fromString(map.value(QStringLiteral("check_in")).toString(), Qt::ISODate);
    res.checkOut          = QDate::fromString(map.value(QStringLiteral("check_out")).toString(), Qt::ISODate);

    // String to Gender
    res.gender            = stringToGender(map.value(QStringLiteral("gender")).toString());

    // Converting back to Int automatically
    res.numberOfGuests    = map.value(QStringLiteral("number_of_guests")).toInt();
    res.roomNumber        = map.value(QStringLiteral("room_number")).toInt();

    // Still need to do the Address struct

    return res;
}