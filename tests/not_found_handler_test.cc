#include <gtest/gtest.h>
#include "request_handler.h"
#include "not_found_handler.h"
#include "utils.h"
#include <string>
#include <vector>
#include <map>
#include "logging_buffer.h"
#include <queue>

namespace http = boost::beast::http;

TEST(NotFoundHandlerTest, RequestHandlerTest) {
    std::queue<BufferEntry> q1;
    std::queue<BufferEntry> q2;
    LoggingBuffer* lb;

    auto empty_request = http::request<http::vector_body<char>>();
    auto response = NotFoundHandler(lb).handle_request(empty_request);

    std::string expected_response_body_string = "404 Not Found\r\n\r\n";
    std::vector<char> expected_response_body_vector(expected_response_body_string.begin(), expected_response_body_string.end());

    GTEST_LOG_(INFO) << "Reason is: " << response.reason();

    EXPECT_EQ(response.result_int(), 404);
    EXPECT_TRUE(response.reason() == "Not Found");
    EXPECT_TRUE(response.body() == expected_response_body_vector);
}

TEST(NotFoundHandlerTest, Factory) {
    std::queue<BufferEntry> q1;
    std::queue<BufferEntry> q2;
    LoggingBuffer* lb;

    auto factory_genereated_handler = NotFoundHandlerFactory::create(lb);

    EXPECT_TRUE(factory_genereated_handler != nullptr);
}