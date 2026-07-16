#include "../hotel/mainwindow.h"

#include <QApplication>
#include "../readers/hotelkamersreader.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    ReadKamers();
    return QApplication::exec();
}
