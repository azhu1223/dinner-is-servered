#ifndef STATIC_HANDLER_H
#define STATIC_HANDLER_H

#include "request_handler.h"
#include "utils.h"
#include "logging_buffer.h"
#include <vector>
#include <string>

namespace http = boost::beast::http;

class StaticHandler : public RequestHandler {
    public: 
        StaticHandler(LoggingBuffer* logging_buffer);
        virtual http::response<http::vector_body<char>> handle_request(const http::request<http::vector_body<char>>& req);
        std::string get_response_content_type(const std::string& file_path);
    private:
        LoggingBuffer* logging_buffer_;
};

class StaticHandlerFactory{
    public:
        static RequestHandler* create(LoggingBuffer* logging_buffer);
};

#endif