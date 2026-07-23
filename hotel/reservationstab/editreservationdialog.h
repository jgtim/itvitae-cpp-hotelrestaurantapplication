#ifndef EDITRESERVATIONDIALOG_H
#define EDITRESERVATIONDIALOG_H

#include <QDialog>
#include "Reservation/reservation.h"

namespace Ui {
class EditReservationDialog;
}

class EditReservationDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EditReservationDialog(QWidget *parent = nullptr);
    ~EditReservationDialog();

    void setReservationData(const Reservation &res);
    Reservation getReservationData() const;

private slots:
    void on_EditReservationButtonBox_accepted();
    void accept() override;

private:
    Ui::EditReservationDialog *ui;
    Reservation m_currentReservation;
};

#endif // EDITRESERVATIONDIALOG_H
