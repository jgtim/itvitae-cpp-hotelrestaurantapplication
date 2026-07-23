#ifndef ADDRESS_H
#define ADDRESS_H

#include <QString>
#include <QDate>

struct Address {
    QString streetLine1;
    QString streetLine2; // Option for appartment number etc.
    QString city;
    QString stateRegion;
    QString postalCode;
    QString country;


    QString toFormattedString() const;
};

#endif // ADDRESS_H
