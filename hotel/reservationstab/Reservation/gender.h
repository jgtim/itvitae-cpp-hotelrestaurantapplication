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

#endif // GENDER_H
