#ifndef RESERVATIONREPOSITORY_H
#define RESERVATIONREPOSITORY_H

#include <QList>
#include <QSqlDatabase>
#include <QString>
#include "hotel/reservationstab/Reservation/reservation.h"

// Reads every reservation (and its guest) out of the database and rebuilds full
// Reservation structs from them. Both ReservationsTab and RoomsWindow read from here
// so there is only one place doing the actual join between "reservations" and "guests".
QList<Reservation> getAllReservationsFromDatabase(QSqlDatabase db);

// Same data, filtered down to one room.
QList<Reservation> getReservationsForRoom(QSqlDatabase db, const QString& roomNumber);

#endif // RESERVATIONREPOSITORY_H
