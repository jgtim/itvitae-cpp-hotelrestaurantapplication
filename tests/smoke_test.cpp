#include <gtest/gtest.h>

#include "hotel/roomswindow/Room/room.h"

TEST(ParseExcelToBool, AcceptsTruthySpreadsheetValues)
{
    EXPECT_TRUE(parseExcelToBool(" yes "));
    EXPECT_TRUE(parseExcelToBool("TRUE"));
    EXPECT_TRUE(parseExcelToBool("1"));
    EXPECT_FALSE(parseExcelToBool("no"));
}