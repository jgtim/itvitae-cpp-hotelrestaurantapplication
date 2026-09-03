#ifndef ROOMSWINDOW_H
#define ROOMSWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>
#include <QSortFilterProxyModel>
#include <QString>

#include "../../filter/multicolumnfilterproxymodel.h"

namespace Ui {
class RoomsWindow;
}

class RoomsWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit RoomsWindow(QWidget *parent = nullptr);
    ~RoomsWindow();
    void setEditMode(bool b);
    void fillInRoomData(int row);
    void saveRoomData(int row);


private:
    Ui::RoomsWindow *ui;
    QStandardItemModel *model;
    MultiColumnFilterProxyModel *proxyModel;

    void updateRoomColors();
    void filterRooms(const QString &text);
    void importExcelRooms();

private slots:
    void on_roomsTableView_doubleClicked(const QModelIndex &index);
    void on_seeRoomDetails_clicked();
    void on_ReturnButton_clicked();
    void on_EditButton_clicked();
    void on_CancelButton_clicked();
};

#endif // ROOMSWINDOW_H