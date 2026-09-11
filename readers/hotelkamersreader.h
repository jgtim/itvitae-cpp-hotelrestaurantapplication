#ifndef HOTELKAMERSREADER_H
#define HOTELKAMERSREADER_H

#include <xlsxdocument.h>
#include <QDir>

#include "hotel/roomswindow/Room/room.h"

// Define data structure
// struct RoomRecord {
//     int floor;
//     QString roomNumber;
//     QString category;
//     int adults;
//     QString view;
//     bool babyBedPossible;
//     QString bed;
//     QString comments;
//     double pricePerNight;
// };

void CheckIfRoomFileOpens(const QString& filePath);

std::optional<QList<Room>> parseRoomFile(); // Optional so in case it fails to open the file it will return explicit failure state instead of just being empty

// Writes rooms back to the placeholder Excel file, in the same order parseRoomFile() reads
// them in (so rooms[i] overwrites the i-th data row, floor headers left untouched).
// Returns false if the file could not be opened or saved.
bool saveRoomFile(const QList<Room>& rooms);

#endif