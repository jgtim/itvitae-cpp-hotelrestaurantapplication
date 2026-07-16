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

void CheckIfRoomFileOpens();

QList<RoomRecord> parseRoomFile();

#endif