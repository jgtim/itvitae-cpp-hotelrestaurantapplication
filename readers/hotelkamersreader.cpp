#include "readers/hotelkamersreader.h"

#include <QDebug>
#include <QDir>
#include <optional>

// Placeholder storage location. Once this project moves to a database, this constant
// (and this whole file) is what gets replaced - see readers/excelroomrepository.cpp,
// which is the only other place that knows this path.
static const QString kRoomFilePath = QStringLiteral("resources/Hotel kamers plus prices v0.4.xlsx");

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

std::optional<QList<Room>> parseRoomFile() {
    // Built-in QXlsx way to check if file opened successfully
    QXlsx::Document xlsx(kRoomFilePath);

    if (!xlsx.isLoadPackage()) {
        qDebug() << "Failed to parse: File could not be loaded.";
        return std::nullopt; // Returns explicit failure state
    }

    QList<Room> rooms;

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

        // Making currency from price in Excel sheet
        QString priceStr = xlsx.read(row, 8).toString();
        double price = priceStr.remove(QChar(0x20AC)).trimmed().toDouble();

        appendRoom(
            rooms,
            cellA,                                                              // roomNumber
            stringToRoomType(xlsx.read(row, 2).toString()),                     // roomType
            xlsx.read(row, 3).toInt(),                                          // adults
            stringToRoomView(xlsx.read(row, 4).toString()),                     // roomView
            parseExcelToBool(xlsx.read(row, 5).toString()),                     // babyBed
            xlsx.read(row, 6).toString().trimmed(),                             // beds
            xlsx.read(row, 7).toString().trimmed(),                             // comments
            price,                                                              // pricePerNight
            stringToRoomStatus(xlsx.read(row, 9).toString().trimmed()),         // Status
            stringToHouseKeepingStatus(xlsx.read(row, 10).toString().trimmed()) // Housekeeping
        );
    }

    return rooms;
}

bool saveRoomFile(const QList<Room>& rooms) {
    // Open the existing file (not a fresh Document) so floor headers, table headers and
    // any formatting stay exactly as they are - only the data rows get overwritten.
    QXlsx::Document xlsx(kRoomFilePath);

    if (!xlsx.isLoadPackage()) {
        qDebug() << "Failed to save: File could not be loaded.";
        return false;
    }

    int lastRow = xlsx.dimension().rowCount();
    int roomIndex = 0;

    for (int row = 1; row <= lastRow && roomIndex < rooms.size(); ++row) {
        QString cellA = xlsx.read(row, 1).toString().trimmed();

        // Same skip rules as parseRoomFile(): blank rows, floor headers and the table header
        // are layout, not data, and must be left alone.
        if (cellA.isEmpty()) {
            continue;
        }
        if (cellA.contains("First floor", Qt::CaseInsensitive) ||
            cellA.contains("Second floor", Qt::CaseInsensitive) ||
            cellA.contains("Third floor", Qt::CaseInsensitive)) {
            continue;
        }
        if (cellA == "Room nr") {
            continue;
        }

        const Room& room = rooms.at(roomIndex);

        xlsx.write(row, 1, room.roomNumber);
        xlsx.write(row, 2, roomTypeToString(room.roomType));
        xlsx.write(row, 3, room.adults);
        xlsx.write(row, 4, roomViewToString(room.roomView));
        xlsx.write(row, 5, room.babyBed ? "Yes" : "No");
        xlsx.write(row, 6, room.beds);
        xlsx.write(row, 7, room.comments);
        xlsx.write(row, 8, room.pricePerNight);
        xlsx.write(row, 9, roomStatusToString(room.roomStatus));
        xlsx.write(row, 10, houseKeepingStatusToString(room.houseKeepingStatus));

        ++roomIndex;
    }

    if (roomIndex != rooms.size()) {
        qDebug() << "Warning: saveRoomFile() expected" << rooms.size()
                  << "data rows but only found" << roomIndex
                  << "in the sheet - file layout may have changed.";
    }

    if (!xlsx.save()) {
        qDebug() << "Failed to save room file.";
        return false;
    }

    return true;
}