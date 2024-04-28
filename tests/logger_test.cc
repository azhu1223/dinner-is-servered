#include <gtest/gtest.h>
#include "logger.h"

TEST(LoggerTest, Intitialization){
    //Execution
    EXPECT_NO_THROW(initialize_logging(true));
    EXPECT_NO_THROW(initialize_logging(false));
}