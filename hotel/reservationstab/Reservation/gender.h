#ifndef GENDER_H
#define GENDER_H

#include <QString>
#include <QMetaType>

enum class Gender {
    Unspecified,
    Male,
    Female,
    NonBinary,
    Other
};

Q_DECLARE_METATYPE(Gender)

QString genderToString(Gender gender);

Gender stringToGender(const QString &str);

#endif
