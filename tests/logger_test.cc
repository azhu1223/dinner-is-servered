#include <gtest/gtest.h>
#include "logger.h"

TEST(LoggerTest, Intitialization){
    //Execution
    EXPECT_NO_THROW(intialize_logging(true));
    EXPECT_NO_THROW(intialize_logging(false));
}