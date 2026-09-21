#include "hotel/reservationstab/Guest/IDType.h"

QString idTypeToString(IDType type) {
    switch (type) {
    case IDType::Passport:      return "Passport";
    case IDType::DriverLicense: return "Driver's License";
    case IDType::NationalID:    return "National ID";

    case IDType::Other:
    default:                    return "Other";
    }
}

IDType stringToIDType(const QString& str) {
    if (str.compare("Passport", Qt::CaseInsensitive) == 0)
        return IDType::Passport;

    if (str.compare("Driver's License", Qt::CaseInsensitive) == 0 ||
        str.compare("DriverLicense", Qt::CaseInsensitive) == 0 ||
        str.compare("Drivers License", Qt::CaseInsensitive) == 0 ||
        str.compare("Driver License", Qt::CaseInsensitive) == 0)
        return IDType::DriverLicense;

    if (str.compare("National ID", Qt::CaseInsensitive) == 0 ||
        str.compare("NationalID", Qt::CaseInsensitive) == 0)
        return IDType::NationalID;

    return IDType::Other;
}