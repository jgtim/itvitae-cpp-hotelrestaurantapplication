#include "hotelkamersreader.h"

#include <xlsxdocument.h>
#include <QDebug>
#include <QDir>

void ReadKamers()
{
    qDebug() << "Working directory:" << QDir::currentPath();

    QXlsx::Document xlsx(
        QStringLiteral("data/Hotel kamers plus prices v0.4.xlsx")
        );

    if (!xlsx.load()) {
        qDebug() << "Failed to open file";
        return;
    }

    qDebug() << xlsx.read(QStringLiteral("A1")).toString();
}