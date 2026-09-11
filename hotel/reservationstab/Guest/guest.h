#ifndef GUEST_H
#define GUEST_H

#include <QString>

#include "hotel/reservationstab/Reservation/gender.h"
#include "hotel/reservationstab/Reservation/address.h"
#include "hotel/reservationstab/Guest/PaymentMethod.h"
#include "hotel/reservationstab/Guest/IDType.h"

struct Guest {
    QString guestID;

    QString name;
    QString phoneNumber;
    QString email;

    Address address;
    QDate dateOfBirth;
    Gender gender;

    PaymentMethod paymentMethod;
    IDType idType;
};

Q_DECLARE_METATYPE(Guest)

#endif // GUEST_H
