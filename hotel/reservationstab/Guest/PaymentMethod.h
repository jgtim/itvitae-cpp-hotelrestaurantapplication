#ifndef PAYMENTMETHOD_H
#define PAYMENTMETHOD_H

#include <QString>
#include <QMetaType>

enum class PaymentMethod {
    CreditCard,
    DebitCard,
    Cash,
    BankTransfer,
    PayPal,
    Other
};

Q_DECLARE_METATYPE(PaymentMethod)

QString paymentMethodToString(PaymentMethod method);

PaymentMethod stringToPaymentMethod(const QString& str);

#endif
