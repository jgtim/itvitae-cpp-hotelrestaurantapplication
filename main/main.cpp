#include "../hotel/mainwindow.h"
#include "../readers/hotelkamersreader.h"

#include <QApplication>
#include <iostream>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    // Testing File reading
/////////////////////////////////////////////////
    ReadKamers();

    std::cout << std::endl;

    QList<RoomRecord> roomList = parseRoomFile();

    for(int i = 0; i < roomList.size(); i++) {
        qDebug() << roomList.at(i).roomNumber;
    }
/////////////////////////////////////////////////

    return QApplication::exec();
}
