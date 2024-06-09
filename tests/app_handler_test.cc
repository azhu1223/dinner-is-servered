#include <gtest/gtest.h>
#include "app_handler.h"
#include "logging_buffer.h"
#include <boost/beast/http.hpp>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>

namespace http = boost::beast::http;

class AppHandlerTest : public ::testing::Test {
protected:
    std::queue<std::pair<LogSeverity, std::string>> q1;
    std::queue<std::pair<LogSeverity, std::string>> q2;
    LoggingBuffer* logging_buffer;
    AppHandler* handler;

    void SetUp() override {
        logging_buffer = new LoggingBuffer(&q1, &q2);
        handler = new AppHandler(logging_buffer);
    }

    void TearDown() override {
        delete handler;
        delete logging_buffer;
    }

    std::string load_image(const std::string& file_path) {
        std::ifstream file(file_path, std::ios::binary);
        std::ostringstream oss;
        oss << file.rdbuf();
        return oss.str();
    }
};

TEST_F(AppHandlerTest, WriteCallback_AppendsDataToString) {
    std::string target_string;
    const char* data = "Hello, World!";
    size_t data_size = strlen(data);

    size_t result = AppHandler::WriteCallback((void*)data, 1, data_size, &target_string);

    EXPECT_EQ(result, data_size);
    EXPECT_EQ(target_string, data);
}

TEST_F(AppHandlerTest, HandleRequest_GetRequest_ReturnsLandingPage) {
    http::request<http::vector_body<char>> req{http::verb::get, "/app", 11};
    auto response = handler->handle_request(req);
    
    EXPECT_EQ(response.result(), http::status::ok);
    EXPECT_EQ(response[http::field::content_type], "text/html");
}

TEST_F(AppHandlerTest, HandleRequest_InvalidMethod_ReturnsBadRequest) {
    http::request<http::vector_body<char>> req{http::verb::put, "/app", 11};
    auto response = handler->handle_request(req);

    EXPECT_EQ(response.result(), http::status::bad_request);
}

TEST_F(AppHandlerTest, ProcessPost_ValidPostRequest_ReturnsOk) {
    std::string body_content = "Content-Disposition: form-data; name=\"relevant-info\"\r\n\r\ninfo\r\n"
                               "Content-Disposition: form-data; name=\"number-of-images\"\r\n\r\n1\r\n"
                               "Content-Disposition: form-data; name=\"file-upload\"\r\n\r\nimage_data\r\n";
    std::vector<char> body_vector(body_content.begin(), body_content.end());
    http::request<http::vector_body<char>> req{http::verb::post, "/app", 11, std::move(body_vector)};

    auto response = handler->process_post(req);
    
    EXPECT_EQ(response.result(), http::status::ok);
    EXPECT_EQ(response[http::field::content_type], "application/json");
}

TEST_F(AppHandlerTest, ProcessPost_InvalidImageIndex_ReturnsErrorJson) {
    std::string body_content = "Content-Disposition: form-data; name=\"relevant-info\"\r\n\r\ninfo\r\n"
                               "Content-Disposition: form-data; name=\"number-of-images\"\r\n\r\n0\r\n";
    std::vector<char> body_vector(body_content.begin(), body_content.end());
    http::request<http::vector_body<char>> req{http::verb::post, "/app", 11, std::move(body_vector)};

    auto response = handler->process_post(req);

    EXPECT_EQ(response.result(), http::status::ok);
    std::string response_body(response.body().begin(), response.body().end());
    EXPECT_NE(response_body.find("\"best_image_index\":-1"), std::string::npos);
}

TEST_F(AppHandlerTest, GenerateLandingPage_ValidRequest_ReturnsFileContent) {
    http::request<http::vector_body<char>> req{http::verb::get, "/app", 11};
    auto response = handler->generate_landing_page(req);

    EXPECT_EQ(response.result(), http::status::ok);
    EXPECT_EQ(response[http::field::content_type], "text/html");
}

TEST_F(AppHandlerTest, GenerateLandingPage_JavaScriptRequest_ReturnsJavaScript) {
    http::request<http::vector_body<char>> req{http::verb::get, "/app/landing_page.js", 11};
    auto response = handler->generate_landing_page(req);

    EXPECT_EQ(response.result(), http::status::ok);
    EXPECT_EQ(response[http::field::content_type], "application/javascript");
}

TEST_F(AppHandlerTest, GenerateLandingPage_CSSRequest_ReturnsCSS) {
    http::request<http::vector_body<char>> req{http::verb::get, "/app/styles.css", 11};
    auto response = handler->generate_landing_page(req);

    EXPECT_EQ(response.result(), http::status::ok);
    EXPECT_EQ(response[http::field::content_type], "text/css");
}

TEST_F(AppHandlerTest, GenerateLandingPage_InvalidRequest_ReturnsBadRequest) {
    http::request<http::vector_body<char>> req{http::verb::get, "/app/invalid", 11};
    auto response = handler->generate_landing_page(req);

    EXPECT_EQ(response.result(), http::status::bad_request);
}

TEST_F(AppHandlerTest, FactoryCreatesHandler) {
    RequestHandler* handler_from_factory = AppHandlerFactory::create(logging_buffer);
    EXPECT_NE(handler_from_factory, nullptr);
    delete handler_from_factory;
}

TEST_F(AppHandlerTest, ProcessPost_ValidBody_ExtractsRelevantInformation) {
    std::string body_content = "Content-Disposition: form-data; name=\"relevant-info\"\r\n\r\ninfo\r\n"
                               "Content-Disposition: form-data; name=\"number-of-images\"\r\n\r\n1\r\n"
                               "Content-Disposition: form-data; name=\"file-upload\"\r\n\r\nimage_data\r\n";
    std::vector<char> body_vector(body_content.begin(), body_content.end());
    http::request<http::vector_body<char>> req{http::verb::post, "/app", 11, std::move(body_vector)};

    auto response = handler->process_post(req);
    
    std::string response_body(response.body().begin(), response.body().end());
    EXPECT_EQ(response.result(), http::status::ok);
    EXPECT_EQ(response[http::field::content_type], "application/json");
}

TEST_F(AppHandlerTest, ProcessPost_GeneratesCaption) {
    std::string relevant_info = "Sample relevant info for caption";
    std::string image_data = load_image("./images/image.png");

    std::string body_content = "Content-Disposition: form-data; name=\"relevant-info\"\r\n\r\n" + relevant_info + "\r\n"
                               "Content-Disposition: form-data; name=\"number-of-images\"\r\n\r\n1\r\n"
                               "Content-Disposition: form-data; name=\"file-upload\"\r\n\r\n" + image_data + "\r\n";
    std::vector<char> body_vector(body_content.begin(), body_content.end());
    http::request<http::vector_body<char>> req{http::verb::post, "/app", 11, std::move(body_vector)};

    auto response = handler->process_post(req);
    
    std::string response_body(response.body().begin(), response.body().end());
    std::cout << "Response Body: " << response_body << std::endl;
    EXPECT_EQ(response.result(), http::status::ok);
    EXPECT_EQ(response[http::field::content_type], "application/json");
    EXPECT_NE(response_body.find("caption"), std::string::npos);
}
