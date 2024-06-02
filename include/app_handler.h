#ifndef APP_HANDLER_H
#define APP_HANDLER_H

#include "request_handler.h"
#include "logging_buffer.h"
#include "utils.h"
#include <vector>

namespace http = boost::beast::http;

class AppHandler : public RequestHandler {
    public:
        AppHandler(LoggingBuffer* logging_buffer);
        virtual http::response<http::vector_body<char>> handle_request(const http::request<http::vector_body<char>>& req);
        http::response<http::vector_body<char>> generate_landing_page(const http::request<http::vector_body<char>>& req);
        http::response<http::vector_body<char>> process_post(const http::request<http::vector_body<char>>& req);

    private:
        LoggingBuffer* logging_buffer_;
};

class AppHandlerFactory {
    public:
        static RequestHandler* create(LoggingBuffer* logging_buffer);
};

#endif