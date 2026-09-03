#ifndef GUEST_H
#define GUEST_H

#include <QString>
#include "../Reservation/gender.h"
#include "../Reservation/address.h"
#include "PaymentMethod.h"
#include "IDType.h"

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

#endif // GUEST_H
