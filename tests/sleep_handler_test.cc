#include <gtest/gtest.h>
#include "sleep_handler.h"
#include "logging_buffer.h"
#include <boost/beast/http.hpp>
#include <chrono>

namespace http = boost::beast::http;

class SleepHandlerTest : public ::testing::Test {
protected:
    std::queue<std::pair<LogSeverity, std::string>> q1;
    std::queue<std::pair<LogSeverity, std::string>> q2;
    LoggingBuffer* logging_buffer;
    SleepHandler* handler;

    void SetUp() override {
        logging_buffer = new LoggingBuffer(&q1, &q2);
        handler = new SleepHandler(logging_buffer);
    }

    void TearDown() override {
        delete handler;
        delete logging_buffer;
    }
};

TEST_F(SleepHandlerTest, HandleRequest) {
    http::request<http::vector_body<char>> req{http::verb::get, "/", 11};

    auto start = std::chrono::high_resolution_clock::now();
    auto response = handler->handle_request(req);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end - start;

    EXPECT_EQ(response.result(), http::status::ok);
    EXPECT_EQ(response.version(), 11U);
    EXPECT_EQ(response[http::field::content_type], "text/plain");
    EXPECT_EQ(response[http::field::content_length], "0");
    EXPECT_TRUE(response.body().empty());

    EXPECT_GE(elapsed.count(), 1000);

    ASSERT_FALSE(q1.empty());
    auto entry1 = q1.front(); q1.pop();
    EXPECT_EQ(entry1.first, INFO);
    EXPECT_EQ(entry1.second, "Going to sleep");

    ASSERT_FALSE(q1.empty());
    auto entry2 = q1.front(); q1.pop();
    EXPECT_EQ(entry2.first, INFO);
    EXPECT_EQ(entry2.second, "Now awake");
}

TEST_F(SleepHandlerTest, FactoryCreatesHandler) {
    RequestHandler* handler_from_factory = SleepHandlerFactory::create(logging_buffer);
    EXPECT_NE(handler_from_factory, nullptr);
    delete handler_from_factory;
}
