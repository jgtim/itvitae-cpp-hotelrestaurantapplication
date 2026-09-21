#ifndef SELECTTABLEDIALOG_H
#define SELECTTABLEDIALOG_H

#include <QDialog>
#include <QVector>
#include "hotel/restaurantwindow/Restaurant/table.h"

class SelectTableDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SelectTableDialog(const QVector<Table>& tables, QWidget *parent = nullptr);
    ~SelectTableDialog();

    int getSelectedTable() const;

private:
    QWidget *tableCanvas;
    int selectedTableNumber;
    void setupLayout();
};

#endif