#ifndef NOT_FOUND_HANDLER_H
#define NOT_FOUND_HANDLER_H

#include "request_handler.h"
#include "utils.h"
#include <vector>

namespace http = boost::beast::http;

class NotFoundHandler : public RequestHandler {
    public:
        NotFoundHandler();
        //NotFoundHandler(short bytes_transferred, const char data[], ServerPaths server_paths);
        //virtual std::vector<char> create_response();
        virtual http::response<http::vector_body<char>> handle_request(const http::request<http::vector_body<char>>& req);
};

#endif