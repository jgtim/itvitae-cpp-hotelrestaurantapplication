#ifndef EDITRESERVATIONDIALOG_H
#define EDITRESERVATIONDIALOG_H

#include <QDialog>

namespace Ui {
class EditReservationDialog;
}

class EditReservationDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EditReservationDialog(QWidget *parent = nullptr);
    ~EditReservationDialog();

    void setReservationData(const QString &name, const QString &room, const QString &checkIn, const QString &checkOut);
    QString getGuestName() const;
    QString getRoomNumber() const;
    QString getCheckInDate() const;
    QString getCheckOutDate() const;

private:
    Ui::EditReservationDialog *ui;
};

#endif // EDITRESERVATIONDIALOG_H
