#ifndef STATIC_HANDLER_H
#define STATIC_HANDLER_H

#include "request_handler.h"
#include "utils.h"
#include <vector>
#include <string>

namespace http = boost::beast::http;

class StaticHandler : public RequestHandler {
    public: 
        StaticHandler();
        virtual http::response<http::vector_body<char>> handle_request(const http::request<http::vector_body<char>>& req);
        std::string get_response_content_type(const std::string& file_path);
};

class StaticHandlerFactory{
    public:
        static RequestHandler* create();
};

#endif