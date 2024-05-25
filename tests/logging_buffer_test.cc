#include <gtest/gtest.h>
#include "logging_buffer.h"
#include <queue>
#include <chrono>

class LoggingBufferFixture : public ::testing::Test {
    protected:
        std::queue<BufferEntry> queue1;
        std::queue<BufferEntry> queue2;
        LoggingBuffer* loggingBuffer;

        void SetUp() override {
            loggingBuffer = new LoggingBuffer(&queue1, &queue2);
        }

        void TearDown() override {
            delete loggingBuffer;
        }
};

TEST_F(LoggingBufferFixture, AddingMessages) {
    GTEST_LOG_(INFO) << "Adding info test.";
    EXPECT_TRUE(loggingBuffer->addToBuffer(INFO, "test"));
    GTEST_LOG_(INFO) << "Testing message.";
    EXPECT_TRUE(queue1.front().second == "test");
    GTEST_LOG_(INFO) << "Testing severity.";
    EXPECT_EQ(queue1.front().first, INFO);

    queue1.pop();

    GTEST_LOG_(INFO) << "Adding error test.";
    EXPECT_TRUE(loggingBuffer->addToBuffer(ERROR, "test"));
    EXPECT_TRUE(queue1.front().second == "test");
    EXPECT_EQ(queue1.front().first, ERROR);

    queue1.pop();

    GTEST_LOG_(INFO) << "Adding fatal test.";
    EXPECT_TRUE(loggingBuffer->addToBuffer(FATAL, "test"));
    EXPECT_TRUE(queue1.front().second == "test");
    EXPECT_EQ(queue1.front().first, FATAL);

    queue1.pop();
}

TEST_F(LoggingBufferFixture, SwitchQueue) {
    EXPECT_TRUE(loggingBuffer->addToBuffer(INFO, "test"));
    EXPECT_TRUE(loggingBuffer->writeToLog());

    EXPECT_TRUE(loggingBuffer->addToBuffer(INFO, "test"));
    EXPECT_TRUE(queue2.front().second == "test");
    EXPECT_EQ(queue2.front().first, INFO);
    EXPECT_TRUE(loggingBuffer->writeToLog());

    EXPECT_TRUE(loggingBuffer->addToBuffer(INFO, "test"));
    EXPECT_TRUE(queue1.front().second == "test");
    EXPECT_EQ(queue1.front().first, INFO);
    EXPECT_TRUE(loggingBuffer->writeToLog());
}

TEST_F(LoggingBufferFixture, WriteEmptyQueue) {
    auto start = std::chrono::high_resolution_clock::now();
    EXPECT_TRUE(loggingBuffer->writeToLog());
    auto stop = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    EXPECT_GE(duration, WAIT_TIME);
}

TEST_F(LoggingBufferFixture, WriteFilledQueue) {
    EXPECT_TRUE(loggingBuffer->addToBuffer(INFO, "test"));
    EXPECT_TRUE(loggingBuffer->addToBuffer(ERROR, "test"));
    EXPECT_TRUE(loggingBuffer->addToBuffer(FATAL, "test"));

    EXPECT_TRUE(loggingBuffer->writeToLog());
}