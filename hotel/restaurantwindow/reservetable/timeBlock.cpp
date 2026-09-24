#include "hotel/restaurantwindow/reservetable/timeBlock.h"


bool isTimeBlockPassed(const QDate &date, TimeBlock block)
{
    QDate currentDate = QDate::currentDate();

    if (date < currentDate) { return true; }
    if (date > currentDate) { return false; }

    QTime blockEndTime;
    switch (block) {
    case TimeBlock::Morning:
        blockEndTime = QTime(12, 0); // Morning ends at 12:00
        break;
    case TimeBlock::Afternoon:
        blockEndTime = QTime(17, 0); // Afternoon ends at 17:00
        break;
    case TimeBlock::Evening:
        blockEndTime = QTime(22, 0); // Evening ends at 22:00
        break;
    }

    return QTime::currentTime() >= blockEndTime;
}

TimeBlock getCurrentTimeBlock() {
    QTime now = QTime::currentTime();

    if (now < QTime(12, 0)) {
        return TimeBlock::Morning;
    } else if (now < QTime(17, 0)) {
        return TimeBlock::Afternoon;
    } else {
        return TimeBlock::Evening;
    }
}