#include "readers/reservationrepository.h"
#include "readers/reservationserializer.h"

#include <QSqlQuery>
#include <QSqlRecord>

QList<Reservation> getAllReservationsFromDatabase(QSqlDatabase db)
{
    QList<Reservation> reservations;

    QSqlQuery resQuery(db);
    resQuery.exec("SELECT * FROM reservations;");

    int resIdIndex = resQuery.record().indexOf("ReservationID");
    int guestIdIndex = resQuery.record().indexOf("GuestID");
    int numberOfGuestsIndex = resQuery.record().indexOf("NumberOfGuests");
    int roomNumberIndex = resQuery.record().indexOf("RoomNumber");
    int checkInIndex = resQuery.record().indexOf("CheckInDate");
    int checkOutIndex = resQuery.record().indexOf("CheckOutDate");
    int paymentStatusIndex = resQuery.record().indexOf("PaymentStatus");

    QSqlQuery guestQuery(db);

    while (resQuery.next()) {
        QVariantMap resMap;
        resMap[QStringLiteral("reservation_id")] = resQuery.value(resIdIndex).toString();
        resMap[QStringLiteral("number_of_guests")] = resQuery.value(numberOfGuestsIndex).toString();
        resMap[QStringLiteral("room_number")] = resQuery.value(roomNumberIndex).toString();
        resMap[QStringLiteral("check_in")] = resQuery.value(checkInIndex).toString();
        resMap[QStringLiteral("check_out")] = resQuery.value(checkOutIndex).toString();
        resMap[QStringLiteral("payment_status")] = resQuery.value(paymentStatusIndex).toString();

        guestQuery.prepare("SELECT * FROM guests WHERE GuestID = :id;");
        guestQuery.bindValue(":id", resQuery.value(guestIdIndex).toString());
        guestQuery.exec();

        if (guestQuery.next()) {
            resMap[QStringLiteral("guest_name")] = guestQuery.value(guestQuery.record().indexOf("GuestName")).toString();
            resMap[QStringLiteral("guest_phone")] = guestQuery.value(guestQuery.record().indexOf("GuestPhoneNumber")).toString();
            resMap[QStringLiteral("guest_email")] = guestQuery.value(guestQuery.record().indexOf("GuestEmail")).toString();
            resMap[QStringLiteral("address")] = guestQuery.value(guestQuery.record().indexOf("GuestAddress")).toString();
            resMap[QStringLiteral("date_of_birth")] = guestQuery.value(guestQuery.record().indexOf("GuestDateOfBirth")).toString();
            resMap[QStringLiteral("gender")] = guestQuery.value(guestQuery.record().indexOf("GuestGender")).toString();
            resMap[QStringLiteral("payment_method")] = guestQuery.value(guestQuery.record().indexOf("GuestPaymentMethod")).toString();
            resMap[QStringLiteral("id_type")] = guestQuery.value(guestQuery.record().indexOf("GuestIdType")).toString();
        }

        reservations.append(ReservationSerializer::fromVariantMap(resMap));
    }

    return reservations;
}

QList<Reservation> getReservationsForRoom(QSqlDatabase db, const QString& roomNumber)
{
    QList<Reservation> result;

    for (const Reservation &res : getAllReservationsFromDatabase(db)) {
        if (res.roomNumber == roomNumber) {
            result.append(res);
        }
    }

    return result;
}
