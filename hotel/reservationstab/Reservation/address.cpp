#include "hotel/reservationstab/Reservation/address.h"
QString Address::toFormattedString() const {
    QString formatted = streetLine1;

    if (!streetLine2.isEmpty()) {
        formatted += "\n" + streetLine2;
    }

    formatted += QString("\n%1, %2 %3").arg(city, stateRegion, postalCode);

    formatted += "\n" + country;

    return formatted;
}