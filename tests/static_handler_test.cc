#include <gtest/gtest.h>
#include "static_handler.h"
#include <string>
#include <vector>
#include <fstream>

namespace http = boost::beast::http;

// Fixture for setting up the StaticHandler and common utilities
class StaticHandlerTest : public ::testing::Test {
protected:
    // Setup common variables here if needed
    void SetUp() override {
        // Initialize objects used in multiple tests
    }
};

// Test successful file response
TEST_F(StaticHandlerTest, HandlesValidFileRequest) {
    // Create a StaticHandler with a valid file path
    StaticHandler handler(0, nullptr, ServerPaths(), "valid_file_path.html");
    
    // Execute
    std::vector<char> response = handler.create_response();

    // Validate the response contains the expected headers and content
    // (You need to specify what you expect here based on your actual server output)
}

// Test 404 Not Found response
TEST_F(StaticHandlerTest, HandlesFileNotFound) {
    // Create a StaticHandler with a non-existing file path
    StaticHandler handler(0, nullptr, ServerPaths(), "non_existing_file.html");
    
    // Execute
    std::vector<char> response = handler.generate_404_response();

    // Validate the response is a 404
    std::string expected_response = "HTTP/1.1 404 Not Found\r\nContent-Type: text/plain\r\nContent-Length: 17\r\n\r\n404 Not Found\r\n\r\n";
    std::string actual_response(response.begin(), response.end());
    EXPECT_EQ(actual_response, expected_response);
}

// Test 500 Internal Server Error response for unsupported file types
TEST_F(StaticHandlerTest, HandlesUnsupportedFileType) {
    // Create a StaticHandler with an unsupported file type
    StaticHandler handler(0, nullptr, ServerPaths(), "unsupported_file_type.xyz");
    
    // Execute
    std::vector<char> response = handler.generate_500_response();

    // Validate the response is a 500
    std::string expected_response = "HTTP/1.1 500 Internal Server Error\r\nContent-Type: text/plain\r\nContent-Length: 17\r\n\r\n500 Internal Server Error\r\n\r\n";
    std::string actual_response(response.begin(), response.end());
    EXPECT_EQ(actual_response, expected_response);
}

// Test content type determination
TEST_F(StaticHandlerTest, DeterminesContentType) {
    // Create a StaticHandler
    StaticHandler handler(0, nullptr, ServerPaths(), "test.jpeg");
    
    // Execute
    std::string content_type = handler.get_response_content_type("test.jpeg");

    // Validate the content type
    EXPECT_EQ(content_type, "image/jpeg");
}

TEST_F(StaticHandlerTest, RequestHandlerTest) {
    // Create a StaticHandler with a valid file path
    StaticHandler handler = StaticHandler();

    boost::string_view target = "resources/text/oof.txt";

    auto request = http::request<http::vector_body<char>>(http::verb::get, target, 11U);
    
    // Execute
    auto response = handler.handle_request(request);

    std::ifstream file(target.to_string(), std::ios::in | std::ios::binary);

    if (!file.is_open()) {
        GTEST_LOG_(ERROR) << "File could not be opened";
    }

    else {
        GTEST_LOG_(INFO) << "File opened successfully";
    }

    file.seekg(0, std::ios::end);
    std::streampos file_size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<char> file_contents(file_size);

    file.read(file_contents.data(), file_size);
    file.close();

    std::vector<char> response_body = response.body();

    EXPECT_EQ(response.result_int(), 200);
    EXPECT_TRUE(response[http::field::content_type] == "text/plain");
    EXPECT_TRUE(response[http::field::content_length] == std::to_string(file_contents.size()));
    EXPECT_TRUE(file_contents == response_body);
}

