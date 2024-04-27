#ifndef RESPONSE_HANDLER_H
#define RESPONSE_HANDLER_H

#include <vector>
#include <boost/beast/http/parser.hpp>
#include <boost/beast/http.hpp>
#include "config_interpreter.h"



class ResponseHandler{
    public:
        ResponseHandler(short bytes_transferred, const char data[], ServerPaths server_paths);
        std::vector<char> create_response();
    private:
        ServerPaths server_paths_;
        boost::beast::http::request_parser<boost::beast::http::string_body> parser;
        const char* data;
        short bytes_transferred;
        std::vector<char> create_echo_response();
        std::vector<char> create_static_response();
        bool isTargetStatic();
        bool isTargetEcho();

};
#endif