#ifndef ROOMREPOSITORYFACTORY_H
#define ROOMREPOSITORYFACTORY_H

#include <memory>
#include "readers/iroomrepository.h"

// Single place that decides which IRoomRepository implementation the app uses.
// Swapping the Excel placeholder for a real database later is a one-line change here -
// nothing else in the app needs to know or care.
std::unique_ptr<IRoomRepository> createRoomRepository();

#endif // ROOMREPOSITORYFACTORY_H
