#include <gtest/gtest.h>
#include "request_handler.h"
#include "not_found_handler.h"
#include "utils.h"
#include <string>
#include <vector>
#include <map>

namespace http = boost::beast::http;

// TEST(NotFoundHandlerTest, Create404Response) {
//     //Setup
//     std::string request = "GET / HTTP/1.1\r\nHost: localhost\\not\\found\r\nUser-Agent: curl/7.81.0\r\nAccept: */*";
//     std::vector<std::string> echo_paths;
//     std::map<std::string, std::string> static_paths;
//     echo_paths.push_back("echo1");
//     static_paths["static1"] = "/static1";

//     ServerPaths sp;
//     sp.echo_ = echo_paths;
//     sp.static_ = static_paths;
    
//     //Execute
//     NotFoundHandler nfh(request.length(), request.c_str(), sp);

//     std::vector<char> response = nfh.create_response();

//     //Expect Response
//     std::string expected_response_str = "HTTP/1.1 404 Not Found\r\n"
//                            "Content-Type: text/plain\r\n"
//                            "Content-Length: 17\r\n\r\n"
//                            "404 Not Found\r\n\r\n";

//     std::vector<char> expected_response(expected_response_str.begin(), expected_response_str.end());
//     GTEST_LOG_(INFO) << "Response is: " << std::string(response.begin(), response.end());
//     GTEST_LOG_(INFO) << "Expected is: " << std::string(expected_response.begin(), expected_response.end());
//     EXPECT_TRUE(response == expected_response);
// }

TEST(NotFoundHandlerTest, RequestHandlerTest) {
    auto empty_request = http::request<http::vector_body<char>>();
    auto response = NotFoundHandler().handle_request(empty_request);

    std::string expected_response_body_string = "404 Not Found\r\n\r\n";
    std::vector<char> expected_response_body_vector(expected_response_body_string.begin(), expected_response_body_string.end());

    GTEST_LOG_(INFO) << "Reason is: " << response.reason();

    EXPECT_EQ(response.result_int(), 404);
    EXPECT_TRUE(response.reason() == "Not Found");
    EXPECT_TRUE(response.body() == expected_response_body_vector);
}