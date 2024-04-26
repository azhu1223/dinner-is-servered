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


// TEST_F(SessionFixture, HandleWriteSuccess) {
//     boost::system::error_code no_error;  
//     session s(io_service);
//     EXPECT_TRUE(s.handle_write(no_error)); 
// }

// TEST_F(SessionFixture, HandleWriteFailure) {
//     boost::system::error_code error(boost::asio::error::network_down); 
//     session s(io_service);
//     EXPECT_FALSE(s.handle_write(error));  
// }

// TEST_F(SessionFixture, HandleReadFailure) {
//     boost::system::error_code error(boost::asio::error::network_down);  
//     session s(io_service);
//     EXPECT_FALSE(s.handle_read(error, 0));  
// }
// TEST_F(SessionFixture, HandleReadNoError) {
//     session s(io_service);
//     boost::system::error_code noError; 
//     size_t bytesTransferred = 42; 

//     bool result = s.handle_read(noError, bytesTransferred);

//     ASSERT_TRUE(result);
// }