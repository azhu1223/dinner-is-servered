#include <gtest/gtest.h>
#include "request_handler.h"
#include "echo_handler.h"
#include "utils.h"
#include <boost/beast/http.hpp>
#include <string>
#include <vector>
#include <map>
#include "config_interpreter.h"


namespace http = boost::beast::http;

TEST(EchoHandlerTest, RequestHandlerTest) {
    //Setup
    std::vector<std::string> echo_paths;
    std::map<std::string, std::string> static_paths;
    echo_paths.push_back("echo1");
    static_paths["static1"] = "/static1";

    ServerPaths sp;
    sp.echo_ = echo_paths;
    sp.static_ = static_paths;
    ConfigInterpreter::setServerPaths(sp);

    boost::string_view target = "/";

    GTEST_LOG_(INFO) << "Creating request for target " << target;
    http::request<http::vector_body<char>> request(http::verb::get, target, 11U);

    GTEST_LOG_(INFO) << "Setting fields of the request";
    request.set(http::field::host, "localhost\\echo1\\foo");
    request.set(http::field::user_agent, "curl/7.81.0");
    request.set(http::field::accept, "*/*");

    GTEST_LOG_(INFO) << "Creating EchoHandler";
    EchoHandler eh = EchoHandler();

    GTEST_LOG_(INFO) << "Invoking handle_request";
    auto response = eh.handle_request(request);
    auto response_header = response.base();
    auto response_body = response.body();

    std::string request_string = "GET / HTTP/1.1\r\nHost: localhost\\echo1\\foo\r\nUser-Agent: curl/7.81.0\r\nAccept: */*\r\n\r\n";
    std::vector<char> request_vector(request_string.begin(), request_string.end());

    GTEST_LOG_(INFO) << "Testing conditions";
    EXPECT_EQ(response.result_int(), 200U);
    EXPECT_TRUE(response[http::field::content_type] == "text/plain");
    EXPECT_TRUE(response[http::field::content_length] == "83");
    EXPECT_TRUE(response.body() == request_vector);
    EXPECT_TRUE(response.reason() == "OK");
}

TEST(EchoHandlerTest, Factory) {

    auto factory_genereated_handler = EchoHandlerFactory::create();

    EXPECT_TRUE(factory_genereated_handler != nullptr);
}