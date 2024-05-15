#include "request_dispatcher.h"
#include "gtest/gtest.h"
#include "config_interpreter.h"

class DispatcherFixture : public ::testing::Test {
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


TEST_F(DispatcherFixture, GetEchoRequestType) {

    boost::string_view target = "/test/echo1/foo.txt";
    GTEST_LOG_(INFO) << "Creating request for target " << target;
    boost::beast::http::request<boost::beast::http::vector_body<char>> request(boost::beast::http::verb::get, target, 11U);
    EXPECT_TRUE(RequestDispatcher::getRequestType(request) == RequestType::Echo);
}

TEST_F(DispatcherFixture, GetStaticRequestType) {

    boost::string_view target = "/staticroot"; //Just using root instead of real file
    GTEST_LOG_(INFO) << "Creating request for target " << target;
    boost::beast::http::request<boost::beast::http::vector_body<char>> request(boost::beast::http::verb::get, target, 11U);
    EXPECT_TRUE(RequestDispatcher::getRequestType(request) == RequestType::Static);
}


TEST_F(DispatcherFixture, Get404RequestType) {

    boost::string_view target = "/this/path/isnt/real";
    GTEST_LOG_(INFO) << "Creating request for target " << target;
    boost::beast::http::request<boost::beast::http::vector_body<char>> request(boost::beast::http::verb::get, target, 11U);
    EXPECT_TRUE(RequestDispatcher::getRequestType(request) == RequestType::None);
}

TEST_F(DispatcherFixture, Get404RequestTypeOnStaticPath) {

    boost::string_view target = "/test/static1/notrealfile";
    GTEST_LOG_(INFO) << "Creating request for target " << target;
    boost::beast::http::request<boost::beast::http::vector_body<char>> request(boost::beast::http::verb::get, target, 11U);
    EXPECT_TRUE(RequestDispatcher::getRequestType(request) == RequestType::None);
}

TEST_F(DispatcherFixture, GetStaticFilePath) {

    boost::string_view target = "/staticroot"; //Just using root instead of real file
    GTEST_LOG_(INFO) << "Creating request for target " << target;
    boost::beast::http::request<boost::beast::http::vector_body<char>> request(boost::beast::http::verb::get, target, 11U);
    EXPECT_TRUE(RequestDispatcher::getStaticFilePath(request) == "/");
}
