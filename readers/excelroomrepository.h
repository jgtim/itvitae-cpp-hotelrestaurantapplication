#ifndef EXCELROOMREPOSITORY_H
#define EXCELROOMREPOSITORY_H

#include "iroomrepository.h"

// Placeholder implementation of IRoomRepository: reads and writes the bundled Excel sheet
// via the existing hotelkamersreader functions. This whole class is what gets swapped out
// (for e.g. a SqliteRoomRepository) once a real database is in place - see
// readers/roomrepositoryfactory.cpp.
class ExcelRoomRepository : public IRoomRepository
{
public:
    std::optional<QList<Room>> loadRooms() override;
    bool saveRooms(const QList<Room>& rooms) override;
};

#endif // EXCELROOMREPOSITORY_H
