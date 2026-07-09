#ifndef HOMETAB_H
#define HOMETAB_H

#include <QWidget>

namespace Ui {
class HomeTab;
}

class HomeTab : public QWidget
{
    Q_OBJECT

public:
    explicit HomeTab(QWidget *parent = nullptr);
    ~HomeTab();

public slots:
    void onDataChanged();

signals:
    void dataChanged();

private:
    Ui::HomeTab *ui;
};

#endif // HOMETAB_H
