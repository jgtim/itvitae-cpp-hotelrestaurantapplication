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

QString paymentStatusToString(PaymentStatus status);

PaymentStatus stringToPaymentStatus(const QString& str);

#endif
