#ifndef SLEEP_HANDLER_H
#define SLEEP_HANDLER_H

#include "request_handler.h"
#include "utils.h"
#include <vector>
#include "logging_buffer.h"
#include <vector>

namespace http = boost::beast::http;

class SleepHandler : public RequestHandler {
    public:
        SleepHandler(LoggingBuffer* logging_buffer);
        virtual http::response<http::vector_body<char>> handle_request(const http::request<http::vector_body<char>>& req);
    private:
        LoggingBuffer* logging_buffer_;
};

class SleepHandlerFactory {
    public:
        static RequestHandler* create(LoggingBuffer* logging_buffer);
};

#endif