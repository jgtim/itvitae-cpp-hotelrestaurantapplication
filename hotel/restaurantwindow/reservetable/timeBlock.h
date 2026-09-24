#ifndef TIMEBLOCK_H
#define TIMEBLOCK_H

#include <QString>
#include <QMetaType>
#include <QDate>

enum class TimeBlock {
    Morning,
    Afternoon,
    Evening
};

Q_DECLARE_METATYPE(TimeBlock);

inline QString timeBlockToString(TimeBlock block) {
    switch (block) {
    case TimeBlock::Morning:   return "Morning";
    case TimeBlock::Afternoon: return "Afternoon";
    case TimeBlock::Evening:   return "Evening";
    }
    return "";
}

inline TimeBlock stringToTimeBlock(const QString &str) {
    QString text = str.trimmed().toLower();

    if (text == "morning")   return TimeBlock::Morning;
    if (text == "afternoon") return TimeBlock::Afternoon;
    if (text == "evening")   return TimeBlock::Evening;

    return TimeBlock::Morning;
}

bool isTimeBlockPassed(const QDate &date, TimeBlock block);
TimeBlock getCurrentTimeBlock();

#endif // TIMEBLOCK_H
