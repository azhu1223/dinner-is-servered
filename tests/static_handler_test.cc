#include <gtest/gtest.h>
#include "static_handler.h"
#include <string>
#include <vector>
#include <fstream>
#include "config_interpreter.h"

namespace http = boost::beast::http;

// Fixture for setting up the StaticHandler and common utilities
class StaticHandlerTest : public ::testing::Test {
protected:
    // Setup common variables here if needed
    void SetUp() override {
        // Initialize objects used in multiple tests
    }
};

// Test content type determination
TEST_F(StaticHandlerTest, DeterminesContentType) {
    // Create a StaticHandler
    StaticHandler handler;

    // Validate the content type
    EXPECT_EQ(handler.get_response_content_type("test.jpeg"), "image/jpeg");
    EXPECT_EQ(handler.get_response_content_type("test.html"), "text/html");
    EXPECT_EQ(handler.get_response_content_type("test.jpg"), "image/jpeg");
    EXPECT_EQ(handler.get_response_content_type("test.txt"), "text/plain");
    EXPECT_EQ(handler.get_response_content_type("test.zip"), "application/zip");
    EXPECT_EQ(handler.get_response_content_type("nofileextension"), "");
    EXPECT_EQ(handler.get_response_content_type("test.notsupportedfileextension"), "");


}

// TEST_F(StaticHandlerTest, RequestHandlerTest) {
//     //Setup
//     std::vector<std::string> echo_paths;
//     std::map<std::string, std::string> static_paths;
//     echo_paths.push_back("echo1");
//     static_paths["/"] = "/";
//     ServerPaths sp;
//     sp.echo_ = echo_paths;
//     sp.static_ = static_paths;
//     ConfigInterpreter::setServerPaths(sp);


//     // Create a StaticHandler with a valid file path
//     StaticHandler handler;

//     boost::string_view target = "/resources/text/oof.txt";

//     auto request = http::request<http::vector_body<char>>(http::verb::get, target, 11U);
    
//     // Execute
//     auto response = handler.handle_request(request);

//     std::ifstream file(target.to_string(), std::ios::in | std::ios::binary);

//     if (!file.is_open()) {
//         GTEST_LOG_(ERROR) << "File could not be opened";
//     }

//     else {
//         GTEST_LOG_(INFO) << "File opened successfully";
//     }

//     file.seekg(0, std::ios::end);
//     std::streampos file_size = file.tellg();
//     file.seekg(0, std::ios::beg);

//     std::vector<char> file_contents(file_size);

//     file.read(file_contents.data(), file_size);
//     file.close();

//     std::vector<char> response_body = response.body();

//     EXPECT_EQ(response.result_int(), 200);
//     EXPECT_TRUE(response[http::field::content_type] == "text/plain");
//     EXPECT_TRUE(response[http::field::content_length] == std::to_string(file_contents.size()));
//     EXPECT_TRUE(file_contents == response_body);
// }

TEST_F(StaticHandlerTest, Factory) {

    auto factory_genereated_handler = StaticHandlerFactory::create();

    EXPECT_TRUE(factory_genereated_handler != nullptr);
}