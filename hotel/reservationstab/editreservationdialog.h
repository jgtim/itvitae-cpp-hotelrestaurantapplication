#ifndef EDITRESERVATIONDIALOG_H
#define EDITRESERVATIONDIALOG_H

#include <QDialog>
#include "Reservation/reservation.h"
#include "../../readers/hotelkamersreader.h"

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
    void setReadOnly(bool readOnly);
    void setAvailableRooms(const QList<Room> &rooms);
    void setExistingReservations(const QList<Reservation> &reservations);

private slots:
    void on_EditReservationButtonBox_accepted();
    void accept() override;
    void updateRoomAvailability();

private:
    Ui::EditReservationDialog *ui;
    Reservation m_currentReservation;
    // QList<QString> m_availableRoomNumbers;
    QList<Room> m_allRooms;
    QList<Reservation> m_allReservations;

    void populateRoomDropdown(const QList<Room> &rooms);
};

#endif // EDITRESERVATIONDIALOG_H
