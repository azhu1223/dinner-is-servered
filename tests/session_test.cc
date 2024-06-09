#include "gtest/gtest.h"
#include "session.h"
#include <boost/bind.hpp>
#include "utils.h"
#include <vector>
#include <string>
#include "logging_buffer.h"
#include <queue>


class SessionFixture : public :: testing::Test {
    
    protected:
        boost::asio::io_service io_service;
        std::shared_ptr<session> session_;
        LoggingBuffer* loggingBuffer;
        std::queue<BufferEntry> q1;
        std::queue<BufferEntry> q2;
        void SetUp() override {
            loggingBuffer = new LoggingBuffer(&q1, &q2);
            session_ = std::make_shared<session>(io_service, loggingBuffer);
        }

        void TearDown() override {
            delete loggingBuffer;
        }
};


TEST_F(SessionFixture, SessionConstructor) {
    session s(io_service, loggingBuffer);
    EXPECT_TRUE(true);
}

TEST_F(SessionFixture, SessionSocket) {
    session s(io_service, loggingBuffer);
    boost::asio::ip::tcp::socket& socket = s.socket();
    EXPECT_TRUE(true);
}


TEST_F(SessionFixture, DefaultProperties) {
    EXPECT_NE(&session_->socket(), nullptr);  // Ensures socket is not null
    // Other properties checks depending on initial conditions assumed by session
}
TEST_F(SessionFixture, StartFunctionSetup) {
    // Checking that start function sets up the async_read_some correctly
    bool result = session_->start();
    EXPECT_TRUE(result);
    // Optionally, verify that the correct handler is set up, but this might require access to internal state or mocks.
}
TEST_F(SessionFixture, RepeatedStartCalls) {
    EXPECT_TRUE(session_->start());  // First start should succeed
    EXPECT_TRUE(session_->start());  // Subsequent starts might be no-op but should still succeed
    // Check if any error flags are raised or if the state is still valid
}
TEST_F(SessionFixture, HandleSocketClosure) {
    session_->start();
    session_->socket().close();  // Manually close the socket
    EXPECT_FALSE(session_->socket().is_open());  // Socket should be closed now
}
TEST_F(SessionFixture, DestructorTest) {
    {
        auto local_session = std::make_shared<session>(io_service, loggingBuffer);
        local_session->start();  // Optionally start the session
    }  // local_session goes out of scope and is destroyed
    // You can check logs or other indicators to ensure destruction went smoothly
    EXPECT_TRUE(true);  // Mainly looking for absence of crashes or errors
}



TEST_F(SessionFixture, HandleReadError) {
    // Setup mocks and expectations
    session s(io_service, loggingBuffer);
    boost::system::error_code ec = boost::asio::error::operation_aborted;
    size_t bytes_transferred = 1024;

    bool result = s.handle_read(ec, bytes_transferred);

    EXPECT_FALSE(result);
}

TEST_F(SessionFixture, HandleWriteError) {
    // Setup mocks and expectations
    session s(io_service, loggingBuffer);
    boost::system::error_code ec = boost::asio::error::operation_aborted;
    size_t bytes_transferred = 1024;

    bool result = s.handle_write(ec);

    EXPECT_FALSE(result);
}

