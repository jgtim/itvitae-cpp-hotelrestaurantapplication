#include "hotelkamersreader.h"

#include <QDebug>

void CheckIfRoomFileOpens() // This was to test
{
    qDebug() << "Working directory:" << QDir::currentPath();

    QXlsx::Document xlsx(
        QStringLiteral("resources/Hotel kamers plus prices v0.4.xlsx")
        );

    if (!xlsx.load()) {
        qDebug() << "Failed to open room file";
        return;
    }

    qDebug() << xlsx.read(QStringLiteral("A1")).toString();
}

QList<RoomRecord> parseRoomFile(){
    QXlsx::Document xlsx(
        QStringLiteral("resources/Hotel kamers plus prices v0.4.xlsx")
        );
    QList<RoomRecord> rooms;

    int currentFloor = 1;
    int lastRow = xlsx.dimension().rowCount();

    for(int row = 1; row <= lastRow; ++row){
        QString cellA = xlsx.read(row, 1).toString().trimmed();

        // Skip empty rows
        if(cellA.isEmpty()){
            continue;
        }

        // Check for floor header, then skip to the next row since there is no data on this one
        if(cellA.contains("First floor", Qt::CaseInsensitive)){
            currentFloor = 1;
            continue;
        }

        if(cellA.contains("Second floor", Qt::CaseInsensitive)){
            currentFloor = 2;
            continue;
        }

        if(cellA.contains("Third floor", Qt::CaseInsensitive)){
            currentFloor = 3;
            continue;
        }

        // Skip table headers
        if(cellA == "Room nr"){
            continue;
        }


        RoomRecord room;
        room.floor = currentFloor;
        room.roomNumber = cellA;
        room.category = xlsx.read(row, 2).toString();
        room.adults = xlsx.read(row, 3).toInt();
        room.view = xlsx.read(row, 4).toString();
        room.babyBedPossible = (xlsx.read(row, 5).toString().toLower() == "yes");
        room.bed = xlsx.read(row, 6).toString();
        room.comments = xlsx.read(row, 7).toString();

        // Turn price into double
        QString priceStr = xlsx.read(row, 9).toString();
        room.pricePerNight = priceStr.remove(QChar(0x20AC)).trimmed().toDouble();

        rooms.append(room);
    }

    return rooms;
}