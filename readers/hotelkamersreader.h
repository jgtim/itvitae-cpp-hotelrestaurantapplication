#ifndef HOTELKAMERSREADER_H
#define HOTELKAMERSREADER_H

#include <xlsxdocument.h>
#include <QDir>

// Define data structure
struct RoomRecord {
    int floor;
    QString roomNumber;
    QString category;
    int adults;
    QString view;
    bool babyBedPossible;
    QString bed;
    QString comments;
    double pricePerNight;
};

void CheckIfFileOpens(const QString& filePath);

std::optional<QList<RoomRecord>> parseRoomFile(); // Optional so in case it fails to open the file it will return explicit failure state instead of just being empty

#endif