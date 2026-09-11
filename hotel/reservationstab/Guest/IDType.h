#ifndef IDTYPE_H
#define IDTYPE_H

#include <QString>
#include <QMetaType>

enum class IDType {
    Passport,
    DriverLicense,
    NationalID,
    Other
};

Q_DECLARE_METATYPE(IDType)

QString idTypeToString(IDType type);
IDType stringToIDType(const QString& str);

#endif
