#ifndef RESERVETABLE_H
#define RESERVETABLE_H

#include <QDialog>

namespace Ui {
class reservetable;
}

class reservetable : public QDialog
{
    Q_OBJECT

public:
    explicit reservetable(QWidget *parent = nullptr);
    ~reservetable();
    std::vector<QString> getdata();

private slots:
    void on_tableSelectButton_clicked();

    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

private:
    Ui::reservetable *ui;
};

#endif // RESERVETABLE_H
