#ifndef NOT_FOUND_HANDLER_H
#define NOT_FOUND_HANDLER_H

#include "request_handler.h"
#include "utils.h"
#include "logging_buffer.h"
#include <vector>

namespace http = boost::beast::http;

class NotFoundHandler : public RequestHandler {
    public:
        NotFoundHandler(LoggingBuffer* logging_buffer);
        virtual http::response<http::vector_body<char>> handle_request(const http::request<http::vector_body<char>>& req);
    private:
        LoggingBuffer* logging_buffer_;
};

class NotFoundHandlerFactory{
    public:
        static RequestHandler* create(LoggingBuffer* logging_buffer);
};

#endif