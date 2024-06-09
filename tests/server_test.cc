#include "gtest/gtest.h"
#include "server.h"
#include "logging_buffer.h"
#include <queue>

class ServerTestFixture : public ::testing::Test
{
  protected:

    boost::asio::io_service io_service; 
    std::queue<BufferEntry> q1;
    std::queue<BufferEntry> q2;
    LoggingBuffer* lb;
    void SetUp() override {
      lb = new LoggingBuffer(&q1, &q2);
    }

    void TearDown() override {
      delete lb;
    }
};
