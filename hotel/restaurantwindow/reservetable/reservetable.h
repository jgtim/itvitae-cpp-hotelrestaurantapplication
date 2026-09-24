#ifndef RESERVETABLE_H
#define RESERVETABLE_H

#include "hotel/restaurantwindow/reservetable/timeBlock.h"
#include "hotel/restaurantwindow/TableReservations/tableReservation.h"

#include <QDialog>
#include <QSqlDatabase>


namespace Ui {
class reservetable;
}

class reservetable : public QDialog
{
    Q_OBJECT

public:
    explicit reservetable(QWidget *parent = nullptr);
    ~reservetable();
    std::vector<QString> getdata();
    void setTable(int);
    TimeBlock getSelectedTimeBlock() const;
    QDate getSelectedDate() const;
    static QList<TableReservation> getAllTableReservations();
    void updateTimeBlockComboBox(const QDate &date);

private slots:
    void on_tableSelectButton_clicked();

    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

    void accept();

private:
    void saveTableToDatabase(TableReservation res);

    Ui::reservetable *ui;
    QSqlDatabase db;
};

#endif // RESERVETABLE_H
