#ifndef RESPONSE_HANDLER_H
#define RESPONSE_HANDLER_H

#include <vector>
#include <boost/beast/http/parser.hpp>
#include <boost/beast/http.hpp>


class ResponseHandler{
    public:
        ResponseHandler(short bytes_transferred, const char data[]);
        std::vector<char> create_response();
    private:
        boost::beast::http::request_parser<boost::beast::http::string_body> parser;
        const char* data;
        short bytes_transferred;
        std::vector<char> create_echo_response();
        std::vector<char> create_static_response();
        bool isTargetStatic();
};
#endif