#ifndef ROOMSWINDOW_H
#define ROOMSWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>
#include <QSortFilterProxyModel>
#include <QString>

namespace Ui {
class RoomsWindow;
}

class RoomsWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit RoomsWindow(QWidget *parent = nullptr);
    ~RoomsWindow();


private:
    Ui::RoomsWindow *ui;
    QStandardItemModel *model;
    QSortFilterProxyModel *proxyModel;

    void updateRoomColors();
    void filterRooms(const QString &text);
    void importExcelRooms();

private slots:
    void on_roomsTableView_doubleClicked(const QModelIndex &index);
    void on_seeRoomDetails_clicked();
};

#endif // ROOMSWINDOW_H