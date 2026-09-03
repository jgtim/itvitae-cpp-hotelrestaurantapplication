#ifndef RESTAURANTWINDOW_H
#define RESTAURANTWINDOW_H

#include <QWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QSplitter>
#include <QVBoxLayout>
#include <QHBoxLayout>

namespace Ui {
class RestaurantWindow;
}

class RestaurantWindow : public QWidget
{
    Q_OBJECT

public:
    explicit RestaurantWindow(QWidget *parent = nullptr);
    ~RestaurantWindow();

public slots:
    void onDataChanged();

signals:
    void dataChanged();

private:
    Ui::RestaurantWindow *ui;

    // Main layout components
    QSplitter *mainSplitter;

    // Left Panel - Sidebar
    QWidget *leftPanel;
    QTableWidget *reservationTable;
    QTableWidget *orderTable;
    QPushButton *viewReservationBtn;
    QPushButton *allOrdersBtn;
    QPushButton *newOrderBtn;

    // Right Panel - Canvas
    QWidget *rightPanel;
    QPushButton *setFreeBtn;
    QPushButton *setOccupiedBtn;
    QPushButton *reservationTableBtn;
    QWidget *tableCanvas;

    // Setup functions
    void setupLayout();
    void setupLeftPanel();
    void setupRightPanel();
    void setupReservationTable();
    void setupOrderTable();
    void setupTableButtons();

    // Mock data
    void loadMockReservations();
    void loadMockOrders();

private slots:
    // Button slots - temporary, for now empty
    void onViewReservationClicked();
    void onAllOrdersClicked();
    void onNewOrderClicked();
    void onSetFreeClicked();
    void onSetOccupiedClicked();
    void onReservationTableClicked();
};

#endif // RESTAURANTWINDOW_H