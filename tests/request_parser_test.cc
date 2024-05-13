#include "request_parser.h"
#include "gtest/gtest.h"
#include "config_interpreter.h"

class ParserFixture : public ::testing::Test {
protected:
    ServerPaths kServerPaths;

    void SetUp() override {
        kServerPaths.echo_.push_back("/test/echo1");
        kServerPaths.echo_.push_back("/test/echo2");
        kServerPaths.static_["/test/static1"] = "/test/static1";
        kServerPaths.static_["/test/static2"] = "/test/static2";
        kServerPaths.static_["/staticroot"] = "/";
        ConfigInterpreter::setServerPaths(kServerPaths);
    }

};


TEST_F(ParserFixture, GetEchoRequestType) {

    boost::string_view target = "/test/echo1/foo.txt";
    GTEST_LOG_(INFO) << "Creating request for target " << target;
    boost::beast::http::request<boost::beast::http::vector_body<char>> request(boost::beast::http::verb::get, target, 11U);
    EXPECT_TRUE(RequestParser::getRequestType(request) == RequestType::Echo);
}

TEST_F(ParserFixture, GetStaticRequestType) {

    boost::string_view target = "/staticroot"; //Just using root instead of real file
    GTEST_LOG_(INFO) << "Creating request for target " << target;
    boost::beast::http::request<boost::beast::http::vector_body<char>> request(boost::beast::http::verb::get, target, 11U);
    EXPECT_TRUE(RequestParser::getRequestType(request) == RequestType::Static);
}


TEST_F(ParserFixture, Get404RequestType) {

    boost::string_view target = "/this/path/isnt/real";
    GTEST_LOG_(INFO) << "Creating request for target " << target;
    boost::beast::http::request<boost::beast::http::vector_body<char>> request(boost::beast::http::verb::get, target, 11U);
    EXPECT_TRUE(RequestParser::getRequestType(request) == RequestType::None);
}

TEST_F(ParserFixture, Get404RequestTypeOnStaticPath) {

    boost::string_view target = "/test/static1/notrealfile";
    GTEST_LOG_(INFO) << "Creating request for target " << target;
    boost::beast::http::request<boost::beast::http::vector_body<char>> request(boost::beast::http::verb::get, target, 11U);
    EXPECT_TRUE(RequestParser::getRequestType(request) == RequestType::None);
}

TEST_F(ParserFixture, GetStaticFilePath) {

    boost::string_view target = "/staticroot"; //Just using root instead of real file
    GTEST_LOG_(INFO) << "Creating request for target " << target;
    boost::beast::http::request<boost::beast::http::vector_body<char>> request(boost::beast::http::verb::get, target, 11U);
    EXPECT_TRUE(RequestParser::getStaticFilePath(request) == "/");
}