#ifndef RESTAURANTRESERVATIONSDETAILSDIALOG_H
#define RESTAURANTRESERVATIONSDETAILSDIALOG_H

#include <QDialog>
#include <QTableWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QDateEdit>
#include <QVector>
#include <QHBoxLayout>
#include "hotel/restaurantwindow/TableReservations/tableReservation.h"

class RestaurantReservationsDetailsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RestaurantReservationsDetailsDialog(QWidget *parent = nullptr);
    ~RestaurantReservationsDetailsDialog();

private:
    // UI shiz
    QTableWidget *todayTable;
    QTableWidget *upcomingTable;
    QLineEdit *searchField;
    QDateEdit *dateFilter;
    QPushButton *resetBtn;
    QHBoxLayout *searchLayout;

    // Data shiz
    QVector<TableReservation> allReservations;

    void setupLayout();
    void setupTodayTable();
    void setupUpcomingTable();
    void setupSearchBar();
    void populateTables();
    void filterReservations();

private slots:
    void onSearchTextChanged(const QString &text);
    void onDateFilterChanged();
    void onResetClicked();
};

#endif