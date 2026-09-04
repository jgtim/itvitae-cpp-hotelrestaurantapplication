#ifndef IROOMREPOSITORY_H
#define IROOMREPOSITORY_H

#include <QList>
#include <optional>

#include "../hotel/roomswindow/Room/room.h"

// Abstracts *where* room data lives. Today that's the placeholder Excel file
// (ExcelRoomRepository); later it will be a database (e.g. SqliteRoomRepository).
//
// RoomsWindow (and anything else that needs room data) should only ever talk to this
// interface, never to QXlsx or SQL directly. That way, switching storage backends later
// means: write one new class implementing IRoomRepository, then change the single line in
// readers/roomrepositoryfactory.cpp that decides which one gets created. Nothing that uses
// IRoomRepository needs to change.
class IRoomRepository
{
public:
    virtual ~IRoomRepository() = default;

    // Loads every room from storage. std::nullopt means storage could not be read at all
    // (e.g. the file/database couldn't be opened) - as opposed to an empty-but-valid list.
    virtual std::optional<QList<Room>> loadRooms() = 0;

    // Persists the given rooms, replacing whatever is currently in storage.
    // Returns false if the write failed, in which case storage should be left untouched.
    virtual bool saveRooms(const QList<Room>& rooms) = 0;
};

#endif // IROOMREPOSITORY_H
