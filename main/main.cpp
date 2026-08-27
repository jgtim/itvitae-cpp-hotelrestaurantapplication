#include "../hotel/mainwindow.h"
#include "../hotel/database/databaseutils.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    prepareDatabaseFile();
    w.show();

    return QApplication::exec();
}
