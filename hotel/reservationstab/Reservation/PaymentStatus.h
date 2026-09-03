#ifndef PAYMENTSTATUS_H
#define PAYMENTSTATUS_H

#include <QString>
#include <QMetaType>

enum class PaymentStatus {
    Unpaid,
    Pending,
    PartiallyPaid,
    Paid,
    Refunded,
    Failed
};

Q_DECLARE_METATYPE(PaymentStatus)

inline QString paymentStatusToString(PaymentStatus status) {
    switch (status) {
        case PaymentStatus::Unpaid:        return "Unpaid";
        case PaymentStatus::Pending:       return "Pending";
        case PaymentStatus::PartiallyPaid: return "Partially Paid";
        case PaymentStatus::Paid:          return "Paid";
        case PaymentStatus::Refunded:      return "Refunded";
        case PaymentStatus::Failed:        return "Failed";
    }
    return "Unpaid";
}


inline PaymentStatus stringToPaymentStatus(const QString& str) {
    QString lowerStr = str.toLower();

    if (lowerStr == "pending")              return PaymentStatus::Pending;

    if (lowerStr == "partially paid" ||
        lowerStr == "partiallypaid")        return PaymentStatus::PartiallyPaid;

    if (lowerStr == "paid")                 return PaymentStatus::Paid;
    if (lowerStr == "refunded")             return PaymentStatus::Refunded;
    if (lowerStr == "failed")               return PaymentStatus::Failed;

    return PaymentStatus::Unpaid;
}

#endif
