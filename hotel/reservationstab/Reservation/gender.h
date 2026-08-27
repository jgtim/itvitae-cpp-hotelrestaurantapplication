#ifndef GENDER_H
#define GENDER_H

#include <QMetaType>

enum class Gender {
    Unspecified,
    Male,
    Female,
    NonBinary,
    Other
};

Q_DECLARE_METATYPE(Gender)

inline QString genderToString(Gender gender) {
    switch(gender) {
        case Gender::Female:    return QStringLiteral("Female");
        case Gender::Male:      return QStringLiteral("Male");
        case Gender::NonBinary: return QStringLiteral("NonBinary");
        case Gender::Other:     return QStringLiteral("Other");
        default:                return QStringLiteral("Unspecified");
    }
}

inline Gender stringToGender(const QString &str) {
    static const std::unordered_map<QString, Gender> stringToEnumMap = {
        { QStringLiteral("Male"),       Gender::Male },
        { QStringLiteral("Female"),     Gender::Female },
        { QStringLiteral("NonBinary"),  Gender::NonBinary },
        { QStringLiteral("Other"),      Gender::Other }
    };

    auto it = stringToEnumMap.find(str);
    if (it != stringToEnumMap.end()) {
        return it->second;
    }
    return Gender::Unspecified;
}

#endif // GENDER_H
