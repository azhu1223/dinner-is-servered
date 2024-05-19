#ifndef CRUD_HANDLER_H
#define CRUD_HANDLER_H

#include "request_handler.h"
#include "utils.h"
#include <vector>

namespace http = boost::beast::http;

class CrudHandler : public RequestHandler {
    public:
        CrudHandler();
        virtual http::response<http::vector_body<char>> handle_request(const http::request<http::vector_body<char>>& req);
};

class CrudHandlerFactory{
    public:
        static RequestHandler* create();
};

#endif