#include "roomdetailsdialog.h"
#include "ui_roomdetailsdialog.h"

#include <QFormLayout>
#include <QLabel>

RoomDetailsDialog::RoomDetailsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RoomDetailsDialog)
{
    ui->setupUi(this);

    // go to default layout for now
    if (!layout()) {
        setLayout(new QFormLayout(this));
    }

}

RoomDetailsDialog::~RoomDetailsDialog()
{
    delete ui;
}

void RoomDetailsDialog::setDetails(const QList<QPair<QString, QString>> &details)
{
    QLayout *currentLayout = layout();


    for (const QPair<QString, QString> &pair : details) {
        QString cleanHeader = pair.first.trimmed();

        QLabel *valueLabel = new QLabel(pair.second, this);
        valueLabel->setStyleSheet("font-weight: bold;");

        if (auto formLayout = qobject_cast<QFormLayout*>(currentLayout)) {
            formLayout->addRow(cleanHeader + ":", valueLabel);
        }
    }
}