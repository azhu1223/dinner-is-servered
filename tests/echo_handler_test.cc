#include <gtest/gtest.h>
#include "request_handler.h"
#include "echo_handler.h"
#include "utils.h"
#include <string>
#include <vector>
#include <map>

//Tests that create responses correctly echoes
TEST(EchoHandlerTest, CreateEchoResponse) {
    //Setup
    std::string request = "GET / HTTP/1.1\r\nHost: localhost\\echo1\\foo\r\nUser-Agent: curl/7.81.0\r\nAccept: */*";
    std::vector<std::string> echo_paths;
    std::map<std::string, std::string> static_paths;
    echo_paths.push_back("echo1");
    static_paths["static1"] = "/static1";

    ServerPaths sp;
    sp.echo_ = echo_paths;
    sp.static_ = static_paths;
    
    //Execute
    EchoHandler eh(request.length(), request.c_str(), sp);

    std::vector<char> response = eh.create_response();

    //Expect Response
    const std::string status = "HTTP/1.0 200 OK\r\n";
    const std::string content_type = "Content-Type: text/plain\r\n";
    const std::string content_length_text = "Content-Length: "+ std::to_string(request.size()) + "\r\n\r\n";
    const std::string expected_response_str = status + content_type + content_length_text + request;

    std::vector<char> expected_response(expected_response_str.begin(), expected_response_str.end());
    GTEST_LOG_(INFO) << "Response is: " << std::string(response.begin(), response.end());
    GTEST_LOG_(INFO) << "Expected is: " << std::string(expected_response.begin(), expected_response.end());
    EXPECT_TRUE(response == expected_response);
}