#include <gtest/gtest.h>
#include "request_handler.h"
#include "health_handler.h"
#include "utils.h"
#include <string>
#include <vector>
#include <map>

namespace http = boost::beast::http;

TEST(HealthHandlerTest, RequestHandlerTest) {
    auto empty_request = http::request<http::vector_body<char>>();
    auto response = HealthHandler().handle_request(empty_request);

    EXPECT_EQ(response.result_int(), 200);
}

TEST(HealthHandlerTest, Factory) {

    auto factory_genereated_handler = HealthHandlerFactory::create();

    EXPECT_TRUE(factory_genereated_handler != nullptr);
}