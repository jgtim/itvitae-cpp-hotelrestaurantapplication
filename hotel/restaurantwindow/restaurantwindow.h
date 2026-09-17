#ifndef RESTAURANTWINDOW_H
#define RESTAURANTWINDOW_H

#include "hotel/restaurantwindow/Restaurant/table.h"
#include "hotel/restaurantwindow/TableReservations/tableReservation.h"

#include <QWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QSplitter>
#include <QVBoxLayout>
#include <QHBoxLayout>
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
    void populateReservationTable(const QVector<TableReservation>& reservations);
    Ui::RestaurantWindow *ui;

    // Main layout components
    QSplitter *mainSplitter;

    // Left Panel - Sidebar
    QWidget *leftPanel;
    QTableWidget *reservationTable;
    QPushButton *viewReservationBtn;

    // Right Panel - Canvas
    QWidget *rightPanel;
    QPushButton *setFreeBtn;
    QPushButton *setOccupiedBtn;
    QPushButton *reservationTableBtn;
    QPushButton *outOfServiceBtn;
    QVector<Table> tables;

    // Setup functions
    void setupLayout();
    void setupLeftPanel();
    void setupRightPanel();
    void setupReservationTable();
    void setupTableButtons();
    void loadMockTables();


    // Mock data
    void loadMockReservations();


private slots:
    void onViewReservationClicked();
    void onSetFreeClicked();
    void onSetOccupiedClicked();
    void onReservationTableClicked();
    void onSetOutOfServiceClicked();
};

#endif // RESTAURANTWINDOW_H