#ifndef ROOMSWINDOW_H
#define ROOMSWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>
#include <QSortFilterProxyModel>
#include <QString>
#include <QSqlDatabase>
#include <memory>

#include "../../filter/multicolumnfilterproxymodel.h"
#include "../../hotel/roomswindow/Room/room.h"
#include "../../readers/iroomrepository.h"
#include "Room/room.h"

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
    std::unique_ptr<IRoomRepository> roomRepository;
    QList<Room> rooms; // Kept index-aligned with model's rows
    QSqlDatabase db;

    void updateRoomColors();
    void filterRooms(const QString &text);
    void loadRooms();
    void populateModelRow(int row, const Room &room);
    void importExcelRooms();
    void addRoomToDatabase(const Room& room);
    bool isRoomInDatabase(const Room& room);

private slots:
    void on_roomsTableView_doubleClicked(const QModelIndex &index);
    void on_seeRoomDetails_clicked();
    void on_ReturnButton_clicked();
    void on_EditButton_clicked();
    void on_CancelButton_clicked();
};

#endif // ROOMSWINDOW_H