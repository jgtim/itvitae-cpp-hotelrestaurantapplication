#ifndef RESERVATIONSTATUS_H
#define RESERVATIONSTATUS_H

#include <QMetaType>

enum class ReservationStatus {
    Pending,
    Confirmed,
    Seated,
    Completed,
    Cancelled,
    NoShow
};

inline QString reservationStatusToString(ReservationStatus status) {
    switch (status) {
        case ReservationStatus::Pending:        return "Pending";
        case ReservationStatus::Confirmed:       return "Confirmed";
        case ReservationStatus::Seated:          return "Seated";
        case ReservationStatus::Completed:      return "Completed";
        case ReservationStatus::Cancelled:      return "Cancelled";
        case ReservationStatus::NoShow:        return "NoShow";
    }
    return "Pending";
}

inline ReservationStatus stringToReservationStatus(const QString& str) {
    QString lowerStr = str.toLower();
    if (lowerStr == "pending")   return ReservationStatus::Pending;
    if (lowerStr == "confirmed") return ReservationStatus::Confirmed;
    if (lowerStr == "seated")   return ReservationStatus::Seated;
    if (lowerStr == "completed")   return ReservationStatus::Completed;
    if (lowerStr == "cancelled")   return ReservationStatus::Cancelled;
    if (lowerStr == "noshow")        return ReservationStatus::NoShow;
    return ReservationStatus::NoShow;
}

Q_DECLARE_METATYPE(ReservationStatus);

#endif // RESERVATIONSTATUS_H
