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

    void setDetails(const QList<QPair<QString, QString>> &details);

    QList<QPair<QString, QString>> getDetails() const;

private:
    Ui::RoomDetailsDialog *ui;
    QPushButton *okButton;
    QPushButton *cancelButton;
};

#endif // ROOMDETAILSDIALOG_H
