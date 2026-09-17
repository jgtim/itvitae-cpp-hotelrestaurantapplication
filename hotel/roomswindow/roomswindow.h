#ifndef ROOMSWINDOW_H
#define ROOMSWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>
#include <QSortFilterProxyModel>
#include <QString>
#include <QSqlDatabase>
#include <QListWidget>
#include <memory>

#include "hotel/roomswindow/Room/room.h"

#include "filter/multicolumnfilterproxymodel.h"

#include "readers/iroomrepository.h"


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
    QListWidget *reservationsListWidget = nullptr; // Lives inside ui->existingReservations

    void updateRoomColors();
    void filterRooms(const QString &text);
    void loadRooms();
    void populateModelRow(int row, const Room &room);
    void addRoomToDatabase(const Room& room);
    bool isRoomInDatabase(const Room& room);
    void updateRoomInDatabase(const Room& room);
    void updateRoomPrice(const Room& room);
    Room getRoomFromDatabaseByNumber(const QString& roomNumber);

    void loadReservationsForRoom(const QString& roomNumber);

private slots:
    void on_roomsTableView_doubleClicked(const QModelIndex &index);
    void on_seeRoomDetails_clicked();
    void on_ReturnButton_clicked();
    void on_EditButton_clicked();
    void on_CancelButton_clicked();
    void handleReservationDoubleClicked(QListWidgetItem *item);
};

#endif // ROOMSWINDOW_H