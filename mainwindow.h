#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTabWidget>

// Forward declaration of tab Classes:
class HomeTab;
class ReservationsTab;
class RoomsTab;

QT_BEGIN_NAMESPACE
namespace Ui {class MainWindow;}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QTabWidget *tabWidget;
    HomeTab *homeTab;
    ReservationsTab *reservationsTab;
    RoomsTab *roomsTab;
};

#endif // MAINWINDOW_H
