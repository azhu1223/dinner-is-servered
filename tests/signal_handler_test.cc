#include "signal_handler.h"
#include <gtest/gtest.h>
#include <boost/log/trivial.hpp>


TEST(SignalHandlerTest, Interupt){
    //Setup
    signal (SIGINT, sig_handler);

    //Expect
    EXPECT_DEATH({raise(SIGINT);},"");
}

