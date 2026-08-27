#ifndef RESERVATIONSTAB_H
#define RESERVATIONSTAB_H

#include <QWidget>
#include <QStandardItemModel>
#include <QSortFilterProxyModel>
#include "../../readers/hotelkamersreader.h"
#include "Reservation/reservation.h"


namespace Ui {
class ReservationsTab;
}

class ReservationsTab : public QWidget
{
    Q_OBJECT

public:
    explicit ReservationsTab(QWidget *parent = nullptr);
    ~ReservationsTab();

public slots:
    void onDataChanged();
    void handleCancelReservation();

signals:
    void dataChanged();
    void dataChangedNotification();

private slots:
    void on_ReservationsTabNewReservationButton_clicked();
    void handleViewReservation(const QModelIndex &index);

private:
    Ui::ReservationsTab *ui;
    QStandardItemModel *model; // For now, maybe SQL later
    QSortFilterProxyModel *proxyModel;

    void addReservationToModel(const Reservation &res);
    Reservation getReservationFromIndex(const QModelIndex &index);
    QList<Reservation> getAllReservations() const;

    void addMockData();
    void handleEditReservation();
    QList<RoomRecord> m_availableRooms;
};

#endif // RESERVATIONSTAB_H
