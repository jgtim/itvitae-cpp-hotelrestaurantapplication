#ifndef RESERVATIONSTAB_H
#define RESERVATIONSTAB_H

#include <QWidget>
#include <QStandardItemModel>
#include <QSortFilterProxyModel>

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

private:
    Ui::ReservationsTab *ui;
    QStandardItemModel *model; // For now, maybe SQL later
    QSortFilterProxyModel *proxyModel;

    void addMockData();
    void handleEditReservation();
};

#endif // RESERVATIONSTAB_H
