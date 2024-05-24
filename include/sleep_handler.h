#ifndef SLEEP_HANDLER_H
#define SLEEP_HANDLER_H

#include "request_handler.h"
#include "utils.h"
#include <vector>

namespace http = boost::beast::http;

class SleepHandler : public RequestHandler {
    public:
        SleepHandler();
        virtual http::response<http::vector_body<char>> handle_request(const http::request<http::vector_body<char>>& req);
};

class SleepHandlerFactory {
    public:
        static RequestHandler* create();
};

#endif