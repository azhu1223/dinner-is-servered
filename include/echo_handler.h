#ifndef STATIC_H
#define STATIC_H

#include "request_handler.h"
#include "utils.h"
#include <vector>

namespace http = boost::beast::http;

class EchoHandler : public RequestHandler {
    public:
        EchoHandler();
        virtual http::response<http::vector_body<char>> handle_request(const http::request<http::vector_body<char>>& req);
};

class EchoHandlerFactory{
    public:
        static RequestHandler* create();
};

#endif