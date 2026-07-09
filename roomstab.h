#ifndef ROOMSTAB_H
#define ROOMSTAB_H

#include <QWidget>

namespace Ui {
class RoomsTab;
}

class RoomsTab : public QWidget
{
    Q_OBJECT

public:
    explicit RoomsTab(QWidget *parent = nullptr);
    ~RoomsTab();

public slots:
    void onDataChanged();

signals:
    void dataChanged();

private:
    Ui::RoomsTab *ui;
};

#endif // ROOMSTAB_H
