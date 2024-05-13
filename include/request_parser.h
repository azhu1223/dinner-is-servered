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
        //RequestParser(short bytes_transferred, const char data[], ServerPaths server_paths);
        RequestParser(http::request<http::vector_body<char>> request, ServerPaths server_paths);
        std::string getFilePath();
        boost::beast::string_view getTarget();
        RequestType getRequestType();


    private:
        void setRequestType();
        http::request<http::vector_body<char>> request_;
        boost::beast::string_view target_;
        ServerPaths server_paths_;
        std::string file_path_;
        RequestType request_type_;
};

#endif