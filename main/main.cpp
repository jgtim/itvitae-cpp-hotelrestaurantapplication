#include "../hotel/mainwindow.h"
#include "../readers/hotelkamersreader.h"

#include <QApplication>
#include <iostream>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    ReadKamers();

    std::cout << std::endl;

    QList<RoomRecord> roomList = parseRoomFile();


    return QApplication::exec();
}
