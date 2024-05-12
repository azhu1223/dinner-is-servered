#ifndef STATIC_H
#define STATIC_H

#include "request_handler.h"
#include "utils.h"
#include <vector>

namespace http = boost::beast::http;

class EchoHandler : public RequestHandler {
    public:
        EchoHandler(short bytes_transferred, const char data[], ServerPaths server_paths);
        EchoHandler();
        virtual std::vector<char> create_response();
        virtual http::response<http::vector_body<char>> handle_request(const http::request<http::vector_body<char>>& req);
};

#endif