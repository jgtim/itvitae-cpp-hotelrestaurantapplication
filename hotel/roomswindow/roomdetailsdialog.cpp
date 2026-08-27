#include "roomdetailsdialog.h"
#include "ui_roomdetailsdialog.h"
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>

RoomDetailsDialog::RoomDetailsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RoomDetailsDialog)
{
    ui->setupUi(this);

    if (!layout()) {
        setLayout(new QVBoxLayout(this));
    }

    auto mainLayout = qobject_cast<QVBoxLayout*>(layout());
    if (!mainLayout) {
        mainLayout = new QVBoxLayout(this);
        setLayout(mainLayout);
    }

    auto formLayout = new QFormLayout();
    mainLayout->addLayout(formLayout);

    auto buttonLayout = new QHBoxLayout();
    okButton = new QPushButton("OK", this);
    cancelButton = new QPushButton("Cancel", this);

    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);
    mainLayout->addLayout(buttonLayout);

    connect(okButton, &QPushButton::clicked, this, &QDialog::accept);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
}

RoomDetailsDialog::~RoomDetailsDialog()
{
    delete ui;
}

void RoomDetailsDialog::setDetails(const QList<QPair<QString, QString>> &details)
{
    auto mainLayout = qobject_cast<QVBoxLayout*>(layout());
    if (!mainLayout) return;

    auto formLayout = qobject_cast<QFormLayout*>(mainLayout->itemAt(0)->layout());
    if (!formLayout) return;

    for (const QPair<QString, QString> &pair : details) {
        QString cleanHeader = pair.first.trimmed();
        QLineEdit *valueLineEdit = new QLineEdit(pair.second, this);
        formLayout->addRow(cleanHeader + ":", valueLineEdit);
    }
}

QList<QPair<QString, QString>> RoomDetailsDialog::getDetails() const
{
    QList<QPair<QString, QString>> details;
    auto mainLayout = qobject_cast<QVBoxLayout*>(layout());
    if (!mainLayout) return details;

    auto formLayout = qobject_cast<QFormLayout*>(mainLayout->itemAt(0)->layout());
    if (!formLayout) return details;

    for (int i = 0; i < formLayout->rowCount(); ++i) {
        QLayoutItem *labelItem = formLayout->itemAt(i, QFormLayout::LabelRole);
        QLayoutItem *fieldItem = formLayout->itemAt(i, QFormLayout::FieldRole);

        if (labelItem && fieldItem) {
            QLabel *labelWidget = qobject_cast<QLabel*>(labelItem->widget());
            QLineEdit *fieldWidget = qobject_cast<QLineEdit*>(fieldItem->widget());

            if (labelWidget && fieldWidget) {
                QString header = labelWidget->text();
                if (header.endsWith(":")) {
                    header = header.left(header.length() - 1);
                }
                details.append(qMakePair(header, fieldWidget->text()));
            }
        }
    }
    return details;
}