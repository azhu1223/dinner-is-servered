#ifndef APP_HANDLER_H
#define APP_HANDLER_H

#include "request_handler.h"
#include "logging_buffer.h"
#include "utils.h"
#include <vector>
#include <string>


namespace http = boost::beast::http;

class AppHandler : public RequestHandler {
    public:
        AppHandler(LoggingBuffer* logging_buffer);
        virtual http::response<http::vector_body<char>> handle_request(const http::request<http::vector_body<char>>& req);
        http::response<http::vector_body<char>> generate_landing_page(const http::request<http::vector_body<char>>& req);
        http::response<http::vector_body<char>> process_post(const http::request<http::vector_body<char>>& req);
        static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp);

    private:
        LoggingBuffer* logging_buffer_;
        static std::string get_relevant_information(const std::string& body);
        static std::vector<std::string> get_images(std::string body);
        static int get_best_image_index(const std::vector<std::string>& image_data);
        static std::string generate_caption(const std::string& image, const std::string& relevant_info);


};

class AppHandlerFactory {
    public:
        static RequestHandler* create(LoggingBuffer* logging_buffer);
};

#endif