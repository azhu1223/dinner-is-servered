#ifndef REQUEST_PARSER_H
#define REQUEST_PARSER_H

#include "utils.h"
#include <boost/beast/http/parser.hpp>
#include <boost/beast/http.hpp>
#include <string>

enum RequestType {
    Static,
    Echo,
    None
};

class RequestParser {

    public:
        RequestParser(short bytes_transferred, const char data[], ServerPaths server_paths);
        std::string getFilePath();
        boost::beast::string_view getTarget();
        RequestType getRequestType();


    private:
        void setRequestType();
        boost::beast::http::request_parser<boost::beast::http::string_body> parser;
        ServerPaths server_paths_;
        std::string file_path_;
        RequestType request_type_;
};

#endif