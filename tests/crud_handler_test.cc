#include <gtest/gtest.h>
#include "crud_handler.h"
#include <string>
#include <vector>
#include <fstream>
#include "config_interpreter.h"
#include <gmock/gmock.h>
#include <boost/beast/http.hpp>
#include "request_dispatcher.h"


namespace http = boost::beast::http;

class CrudHandlerTest : public ::testing::Test {
protected:
    void SetUp() override {
        handler = new CrudHandler();
    }

    void TearDown() override {
        delete handler;
    }

    CrudHandler* handler;
};


TEST_F(CrudHandlerTest, Factory) {

    auto factory_genereated_handler = CrudHandlerFactory::create();

    EXPECT_TRUE(factory_genereated_handler != nullptr);
}


TEST_F(CrudHandlerTest, BadRequestMethodTest) {

    http::request<http::vector_body<char>> req;
    req.method(http::verb::connect);
    req.target("/api/Books");
    req.version(11);

    http::response<http::vector_body<char>> response = handler->handle_request(req);

    EXPECT_EQ(response.result(), http::status::bad_request);
}
