#ifndef RESERVATIONSTAB_H
#define RESERVATIONSTAB_H

#include <QWidget>

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
};

#endif // RESERVATIONSTAB_H
