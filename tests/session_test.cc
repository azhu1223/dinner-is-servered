#include "gtest/gtest.h"
#include "session.h"
#include <boost/bind.hpp>


class SessionFixture : public :: testing::Test {
    
    protected:
        boost::asio::io_service io_service;
        std::unique_ptr<session> session_;
        void SetUp() override {
        session_ = std::make_unique<session>(io_service);
        }

};


TEST_F(SessionFixture, SessionConstructor) {
    session s(io_service);
    EXPECT_TRUE(true);
}

TEST_F(SessionFixture, SessionSocket) {
    session s(io_service);
    boost::asio::ip::tcp::socket& socket = s.socket();
    EXPECT_TRUE(true);
}


TEST_F(SessionFixture, StartFunction) {
    session s(io_service);
    bool result = s.start();
    EXPECT_TRUE(result);  
}
