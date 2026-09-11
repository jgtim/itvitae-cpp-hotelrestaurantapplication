#include "hotel/reservationstab/Guest/PaymentMethod.h"

QString paymentMethodToString(PaymentMethod method) {
    switch (method) {
    case PaymentMethod::CreditCard:   return "Credit Card";
    case PaymentMethod::DebitCard:    return "Debit Card";
    case PaymentMethod::Cash:         return "Cash";
    case PaymentMethod::BankTransfer: return "Bank Transfer";
    case PaymentMethod::PayPal:       return "PayPal";
    case PaymentMethod::Other:
    default:                          return "Other";
    }
}

PaymentMethod stringToPaymentMethod(const QString& str) {

    if (str.compare("Credit Card", Qt::CaseInsensitive) == 0 ||
        str.compare("CreditCard", Qt::CaseInsensitive) == 0)
        return PaymentMethod::CreditCard;

    if (str.compare("Debit Card", Qt::CaseInsensitive) == 0 ||
        str.compare("DebitCard", Qt::CaseInsensitive) == 0)
        return PaymentMethod::DebitCard;

    if (str.compare("Cash", Qt::CaseInsensitive) == 0)
        return PaymentMethod::Cash;

    if (str.compare("Bank Transfer", Qt::CaseInsensitive) == 0 ||
        str.compare("BankTransfer", Qt::CaseInsensitive) == 0)
        return PaymentMethod::BankTransfer;

    if (str.compare("PayPal", Qt::CaseInsensitive) == 0)
        return PaymentMethod::PayPal;

    return PaymentMethod::Other;
}