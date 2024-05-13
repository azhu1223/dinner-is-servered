#ifndef REQUEST_PARSER_H
#define REQUEST_PARSER_H

#include "utils.h"
#include <boost/beast/http/parser.hpp>
#include <boost/beast/http.hpp>
#include <string>

namespace http = boost::beast::http;

enum RequestType {
    Static,
    Echo,
    None
};

class RequestParser {

    public:
        static RequestType getRequestType(const boost::beast::http::request<boost::beast::http::vector_body<char>>& request);
        static std::string getStaticFilePath(const boost::beast::http::request<boost::beast::http::vector_body<char>>& request);
};

#endif