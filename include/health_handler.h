#ifndef HEALTH_HANDLER_H
#define HEALTH_HANDLER_H

#include "request_handler.h"
#include "utils.h"
#include "logging_buffer.h"
#include <vector>

namespace http = boost::beast::http;

class HealthHandler : public RequestHandler {
    public:
        HealthHandler(LoggingBuffer* logging_buffer);
        virtual http::response<http::vector_body<char>> handle_request(const http::request<http::vector_body<char>>& req);
    private:
        LoggingBuffer* logging_buffer_;
};

class HealthHandlerFactory {
    public:
        static RequestHandler* create(LoggingBuffer* logging_buffer);
};

#endif