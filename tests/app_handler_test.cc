#include <gtest/gtest.h>
#include "app_handler.h"
#include "logging_buffer.h"
#include <boost/beast/http.hpp>
#include <string>
#include <vector>

namespace http = boost::beast::http;

class AppHandlerTest : public ::testing::Test {
protected:
    std::queue<std::pair<LogSeverity, std::string>> q1;
    std::queue<std::pair<LogSeverity, std::string>> q2;
    LoggingBuffer* logging_buffer;
    AppHandler* handler;

    void SetUp() override {
        logging_buffer = new LoggingBuffer(&q1, &q2);
        handler = new AppHandler(logging_buffer);
    }

    void TearDown() override {
        delete handler;
        delete logging_buffer;
    }
};

TEST_F(AppHandlerTest, HandleRequest_GetRequest_ReturnsLandingPage) {
    http::request<http::vector_body<char>> req{http::verb::get, "/app", 11};
    auto response = handler->handle_request(req);
    
    EXPECT_EQ(response.result(), http::status::ok);
    EXPECT_EQ(response[http::field::content_type], "text/html");
}

TEST_F(AppHandlerTest, HandleRequest_InvalidMethod_ReturnsBadRequest) {
    http::request<http::vector_body<char>> req{http::verb::put, "/app", 11};
    auto response = handler->handle_request(req);

    EXPECT_EQ(response.result(), http::status::bad_request);
}


TEST_F(AppHandlerTest, FactoryCreatesHandler) {
    RequestHandler* handler_from_factory = AppHandlerFactory::create(logging_buffer);
    EXPECT_NE(handler_from_factory, nullptr);
    delete handler_from_factory;
}
