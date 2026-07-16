#ifndef RESERVATIONSTAB_H
#define RESERVATIONSTAB_H

#include <QWidget>
#include <QStandardItemModel>

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

signals:
    void dataChanged();

private:
    Ui::ReservationsTab *ui;
    QStandardItemModel *model; // For now, maybe SQL later
    void addMockData();
    void on_ReservationTabEditButton_clicked();
};

#endif // RESERVATIONSTAB_H
