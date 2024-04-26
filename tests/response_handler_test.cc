// #include <gtest/gtest.h>
// #include "response_handler.h"
// #include <string>
// #include <vector>

// //Tests that create responses correctly echoes
// TEST(ResponseHandlerTest, CreateResponse) {
//     //Setup
//     const std::string request = "GET / HTTP/1.1\r\nHost: localhost\r\nUser-Agent: curl/7.81.0\r\nAccept: */*";
    
//     //Execute
//     std::vector<char> response = create_response(request.size(), request.c_str());

//     //Expect Response
//     const std::string status = "HTTP/1.0 200 OK\r\n";
//     const std::string content_type = "Content-Type: text/plain\r\n";
//     const std::string content_length_text = "Content-Length: "+ std::to_string(request.size()) + "\r\n\r\n";
//     const std::string expected_response_str = status + content_type + content_length_text + request;

//     std::vector<char> expected_response(expected_response_str.begin(), expected_response_str.end());
//     GTEST_LOG_(INFO) << "Response is: " << std::string(response.begin(), response.end());
//     GTEST_LOG_(INFO) << "Expected is: " << std::string(expected_response.begin(), expected_response.end());
//     EXPECT_TRUE(response == expected_response);
// }


///TODO - rewrite tests