#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTabWidget>

// Forward declaration of tab Classes:
class ReservationsTab;
class RoomsWindow;
class RestaurantWindow;

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
    ReservationsTab *reservationsTab;
    RoomsWindow *roomswindow;
    RestaurantWindow *restaurantwindow;

private slots:
    void onTabChanged(int index);
    void handleDataChangedInMainWindow();

signals:
    void dataChangedNotification();
};

#endif // MAINWINDOW_H
