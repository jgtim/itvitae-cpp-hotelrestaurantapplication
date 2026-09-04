#include "roomrepositoryfactory.h"
#include "excelroomrepository.h"

std::unique_ptr<IRoomRepository> createRoomRepository() {
    // TODO: once the database is ready, replace this with e.g.:
    //     return std::make_unique<SqliteRoomRepository>();
    return std::make_unique<ExcelRoomRepository>();
}
