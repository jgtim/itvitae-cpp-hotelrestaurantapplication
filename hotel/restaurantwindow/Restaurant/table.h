#ifndef TABLE_H
#define TABLE_H

#include <QString>

enum class TableStatus {
    FREE,       // Groen
    OCCUPIED,   // Rood
    RESERVED,    // Oranje
    OUT_OF_SERVICE //Grijs
};

class Table
{
public:
    Table(int number, int x, int y, TableStatus status = TableStatus::FREE)
        : number(number), x(x), y(y), status(status) {}

    int getNumber() const { return number; }
    int getX() const { return x; }
    int getY() const { return y; }
    TableStatus getStatus() const { return status; }

    void setStatus(TableStatus newStatus) { status = newStatus; }

private:
    int number;     // 1-31
    int x;          // X positie in canvas
    int y;          // Y positie in canvas
    TableStatus status;
};

#endif // TABLE_H