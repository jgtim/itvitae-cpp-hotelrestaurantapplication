#include "reservationserializer.h"
#include "../hotel/reservationstab/Reservation/gender.h"
#include "../hotel/reservationstab/Reservation/PaymentStatus.h"

#include <QJsonObject>
#include <QJsonDocument>

QVariantMap ReservationSerializer::toVariantMap(const Reservation &res) {
    QVariantMap map;

    // Can use the String data field directly
    map[QStringLiteral("reservation_id")]    = res.ReservationID;
    map[QStringLiteral("guest_name")]        = res.guest.name;
    map[QStringLiteral("guest_phone")]       = res.guest.phoneNumber;
    map[QStringLiteral("guest_email")]       = res.guest.email;
    map[QStringLiteral("room_number")]       = res.roomNumber;

    // Putting the QDate datat type in a ISO-8601 for SQLite
    map[QStringLiteral("date_of_birth")]     = res.guest.dateOfBirth.toString(Qt::ISODate);
    map[QStringLiteral("check_in")]          = res.checkIn.toString(Qt::ISODate);
    map[QStringLiteral("check_out")]         = res.checkOut.toString(Qt::ISODate);

    // Converting enum class to String
    map[QStringLiteral("gender")]            = genderToString(res.guest.gender);
    map[QStringLiteral("payment_status")]           = paymentStatusToString(res.paymentStatus);

    // Putting in the Integers directly
    map[QStringLiteral("number_of_guests")]  = res.numberOfGuests;


    // This might be unneccessary but I couldn't find another way to get everything into one class to use.
    // Putting the Address struct in a JSON for single column representation
    QJsonObject addressObj;
    addressObj[QStringLiteral("street1")]       = res.guest.address.streetLine1;
    addressObj[QStringLiteral("street2")]       = res.guest.address.streetLine2;
    addressObj[QStringLiteral("city")]          = res.guest.address.city;
    addressObj[QStringLiteral("state")]         = res.guest.address.stateRegion;
    addressObj[QStringLiteral("postal_code")]   = res.guest.address.postalCode;
    addressObj[QStringLiteral("country")]       = res.guest.address.country;

    // Convert JSON Object to String for SQLite to put in single colums
    map[QStringLiteral("address")] = QString::fromUtf8(QJsonDocument(addressObj).toJson(QJsonDocument::Compact));

    return map;
}



Reservation ReservationSerializer::fromVariantMap(const QVariantMap &map) {
    Reservation res;

    res.ReservationID      = map.value(QStringLiteral("reservation_id")).toString();
    res.guest.name         = map.value(QStringLiteral("guest_name")).toString();
    res.guest.phoneNumber  = map.value(QStringLiteral("guest_phone")).toString();
    res.guest.email        = map.value(QStringLiteral("guest_email")).toString();

    // ISO-8601 to QDate
    res.guest.dateOfBirth  = QDate::fromString(map.value(QStringLiteral("date_of_birth")).toString(), Qt::ISODate);
    res.checkIn            = QDate::fromString(map.value(QStringLiteral("check_in")).toString(), Qt::ISODate);
    res.checkOut           = QDate::fromString(map.value(QStringLiteral("check_out")).toString(), Qt::ISODate);

    // String to enum class
    res.guest.gender       = stringToGender(map.value(QStringLiteral("gender")).toString());
    res.paymentStatus      = stringToPaymentStatus(map.value(QStringLiteral("payment_status")).toString());

    // Converting back to Int automatically
    res.numberOfGuests     = map.value(QStringLiteral("number_of_guests")).toInt();
    res.roomNumber         = map.value(QStringLiteral("room_number")).toInt();


    // This might be unneccessary but I couldn't find another way to get everything into one class to use.
    // Putting SQLite Strings into JSON object, then into Address struct
    QString addressJsonStr = map.value(QStringLiteral("address")).toString();
    QJsonDocument doc = QJsonDocument::fromJson(addressJsonStr.toUtf8());

    if (doc.isObject()) {
        QJsonObject addressObj = doc.object();
        res.guest.address.streetLine1 = addressObj.value(QStringLiteral("street1")).toString();
        res.guest.address.streetLine2 = addressObj.value(QStringLiteral("street2")).toString();
        res.guest.address.city        = addressObj.value(QStringLiteral("city")).toString();
        res.guest.address.stateRegion = addressObj.value(QStringLiteral("state")).toString();
        res.guest.address.postalCode  = addressObj.value(QStringLiteral("postal_code")).toString();
        res.guest.address.country     = addressObj.value(QStringLiteral("country")).toString();
    }

    return res;
}