#ifndef HEALTH_HANDLER_H
#define HEALTH_HANDLER_H

#include "request_handler.h"
#include "utils.h"
#include <vector>

namespace http = boost::beast::http;

class HealthHandler : public RequestHandler {
    public:
        HealthHandler();
        virtual http::response<http::vector_body<char>> handle_request(const http::request<http::vector_body<char>>& req);
};

class HealthHandlerFactory {
    public:
        static RequestHandler* create();
};

#endif