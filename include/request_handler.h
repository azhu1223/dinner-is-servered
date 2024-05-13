#ifndef REQUEST_HANDLER_H
#define REQUEST_HANDLER_H

#include "utils.h"
#include <boost/beast/http/parser.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/core.hpp> 
#include <vector>

namespace http = boost::beast::http;

class RequestHandler{
    public:
        RequestHandler();
        virtual http::response<http::vector_body<char>> handle_request(const http::request<http::vector_body<char>>& req) = 0; 

};
#endif