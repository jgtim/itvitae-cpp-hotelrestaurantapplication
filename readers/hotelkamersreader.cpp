#include "hotelkamersreader.h"
#include "..\hotel\roomswindow\Room\room.h"

#include <QDebug>
#include <QDir>
#include <optional>

void CheckIfRoomFileOpens(const QString& filePath)
{
    qDebug() << "Working directory:" << QDir::currentPath();

    QXlsx::Document xlsx(filePath);

    if (!xlsx.isLoadPackage()) {
        qDebug() << "Failed to open file";
        return;
    }

    qDebug() << "Succesfully opened file at: " << filePath;
}

std::optional<QList<RoomRecord>> parseRoomFile(){
    // Built-in QXlsx way to check if file opened successfully
    QXlsx::Document xlsx(QStringLiteral("resources/Hotel kamers plus prices v0.4.xlsx"));
    if (!xlsx.isLoadPackage()) {
        qDebug() << "Failed to parse: File could not be loaded.";
        return std::nullopt; // Returns explicit failure state
    }

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
        QString priceStr = xlsx.read(row, 8).toString();
        room.pricePerNight = priceStr.remove(QChar(0x20AC)).trimmed().toDouble();

        rooms.append(room);
    }

    return rooms;
}