#ifndef TABLECANVAS_H
#define TABLECANVAS_H

#include <QWidget>

#include "hotel/restaurantwindow/Restaurant/table.h"

class TableCanvas : public QWidget
{Q_OBJECT
public:
    TableCanvas(const QVector<Table>& tables, QWidget *parent = nullptr)
    : QWidget(parent), tables(tables) {
        updateCanvasSize();
    }
    void updateCanvasSize();
    void updateTables(QVector<Table> newTables);
    int getSelectedTable();
    QVector<Table>* getTables();
protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
private:
    QVector<Table> tables;
    int selectedTable = 0;

    void onTableClicked(const Table& table);
    void paintTable(const Table& table, QPainter& painter);
    void saveTables(const QVector<Table> &vector);
    QVector<Table> loadTables();
};

#endif // TABLECANVAS_H
