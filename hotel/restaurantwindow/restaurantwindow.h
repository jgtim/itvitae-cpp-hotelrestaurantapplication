#ifndef RESTAURANTWINDOW_H
#define RESTAURANTWINDOW_H

#include <QWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QSplitter>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "Restaurant/table.h"
#include <QVector>

namespace Ui {
class RestaurantWindow;
}

class TableCanvas;

class RestaurantWindow : public QWidget
{
    Q_OBJECT

public:
    explicit RestaurantWindow(QWidget *parent = nullptr);
    ~RestaurantWindow();
    TableCanvas* canvasPointer = nullptr;
    QVector<Table>* getTables();
    QTableWidget* getReservationTable();

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
    QVector<Table> tables;

    // Setup functions
    void setupLayout();
    void setupLeftPanel();
    void setupRightPanel();
    void setupReservationTable();
    void setupOrderTable();
    void setupTableButtons();
    void loadMockTables();

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