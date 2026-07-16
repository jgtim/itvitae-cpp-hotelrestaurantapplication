#ifndef ROOMDETAILSDIALOG_H
#define ROOMDETAILSDIALOG_H

#include <QDialog>

namespace Ui {
class RoomDetailsDialog;
}

class RoomDetailsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RoomDetailsDialog(QWidget *parent = nullptr);
    ~RoomDetailsDialog();

    void setDetails(const QMap<QString, QString> &details);

private:
    Ui::RoomDetailsDialog *ui;
};

#endif // ROOMDETAILSDIALOG_H
