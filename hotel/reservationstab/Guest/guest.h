#ifndef GUEST_H
#define GUEST_H

#include <QString>
#include "../Reservation/gender.h"
#include "../Reservation/address.h"

struct Guest {
    QString guestID;

    QString name;
    QString phoneNumber;
    QString email;

    Address address;
    QDate dateOfBirth;
    Gender gender;
};

#endif // GUEST_H
