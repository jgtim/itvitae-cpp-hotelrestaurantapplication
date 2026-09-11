#include "readers/excelroomrepository.h"
#include "readers/hotelkamersreader.h"

std::optional<QList<Room>> ExcelRoomRepository::loadRooms() {
    return parseRoomFile();
}

bool ExcelRoomRepository::saveRooms(const QList<Room>& rooms) {
    return saveRoomFile(rooms);
}
