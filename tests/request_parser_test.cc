#include "request_parser.h"
#include "gtest/gtest.h"
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <string>

namespace http = boost::beast::http;

const short kBytesTransferred = 10;
const char kData[10] = "Test data";
ServerPaths kServerPaths; 


class ParserFixture : public ::testing::Test {
protected:
    ServerPaths kServerPaths;
    ServerPaths xServerPaths;
    

    void SetUp() override {
        kServerPaths.echo_.push_back("/test/echo1");
        kServerPaths.echo_.push_back("/test/echo2");
        kServerPaths.static_["/test/static1"] = "/test/static1";
        kServerPaths.static_["/test/static2"] = "/test/static2";

        xServerPaths.echo_.push_back("/test/echo");
        xServerPaths.static_["/test/static"] = "images/";
    }

    std::unique_ptr<RequestParser> createRequestParser(http::request<http::vector_body<char>> request) {
        return std::make_unique<RequestParser>(request, xServerPaths);
    }

    std::unique_ptr<http::request<http::vector_body<char>>> createRequest(boost::beast::string_view target, boost::beast::string_view host) {
        auto request_ptr = std::make_unique<http::request<http::vector_body<char>>>(http::verb::get, target, 11U);
        request_ptr->set(http::field::host, host);

        return request_ptr;
    }
};


TEST_F(ParserFixture, ParserConstructor) {
    auto request = createRequest("test", "oof");
    auto parser = createRequestParser(*request);

    EXPECT_STREQ(parser->getFilePath().c_str(), "");
}


TEST_F(ParserFixture, EchoRequest_ReturnsTrue) {
    int num = 0;
    for (std::string echo_path : xServerPaths.echo_) {
        GTEST_LOG_(INFO) << "Path " << std::to_string(num++) << " is " << echo_path;
    }

    auto request = createRequest("/test/echo/something", "foo.com");
    GTEST_LOG_(INFO) << "Request target is " << request->target();
    auto parser = createRequestParser(*request);
    GTEST_LOG_(INFO) << "The request type is: " << parser->getRequestType();
    EXPECT_TRUE(parser->getRequestType() == RequestType::Echo);
}

TEST_F(ParserFixture, NonEchoRequest_ReturnsFalse) {
    auto request = createRequest("/test/other/something", "foo.com");
    auto parser = createRequestParser(*request);
    EXPECT_TRUE(parser->getRequestType() == RequestType::None);
}

TEST_F(ParserFixture, StaticRequest_ReturnsTrue) {
    auto request = createRequest("/test/static/image.png", "example.com");
    auto parser = createRequestParser(*request);
    EXPECT_TRUE(parser->getRequestType() == RequestType::Static);
}

TEST_F(ParserFixture, NonStaticRequest_ReturnsFalse) {
    auto request = createRequest("/test/api/data", "example.com");
    auto parser = createRequestParser(*request);
    EXPECT_TRUE(parser->getRequestType() == RequestType::None);
}

TEST_F(ParserFixture, EchoRequestWithConsecutiveSlashes_ReturnsTrue) {
    auto request = createRequest("/test/echo//something", "example.com");
    auto parser = createRequestParser(*request);
    EXPECT_TRUE(parser->getRequestType() == RequestType::Echo);
}

TEST_F(ParserFixture, StaticRequestWithConsecutiveSlashes_ReturnsTrue) {
    auto request = createRequest("/test/static//image.png", "example.com");
    auto parser = createRequestParser(*request);
    EXPECT_TRUE(parser->getRequestType() == RequestType::Static);
}

TEST_F(ParserFixture, GetTarget) {
    const char* requestData = "GET /test/static//image.png HTTP/1.1\r\nHost: example.com\r\n\r\n";
    auto request = createRequest("/test/static//image.png", "example.com");
    auto parser = createRequestParser(*request);

    const std::string expected_target = "/test/static//image.png";
    boost::beast::string_view target= parser->getTarget();
    GTEST_LOG_(INFO) << "Target is " << target.to_string();
    EXPECT_TRUE(expected_target == target.to_string());
}
