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

void RoomDetailsDialog::setDetails(const QMap<QString, QString> &details)
{
    QLayout *currentLayout = layout();

    // Iterating through the details and Header -> Value
    for (auto it = details.constBegin(); it != details.constEnd(); ++it) {
        QString cleanHeader = it.key().trimmed();

        QLabel *valueLabel = new QLabel(it.value(), this);
        valueLabel->setStyleSheet("font-weight: bold;");

        if (auto formLayout = qobject_cast<QFormLayout*>(currentLayout)) {
            formLayout->addRow(cleanHeader + ":", valueLabel);
        }
    }
}