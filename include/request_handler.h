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
        //RequestHandler(short bytes_transferred, const char data[], ServerPaths server_paths);
        //virtual std::vector<char> create_response() = 0;
        virtual http::response<http::vector_body<char>> handle_request(const http::request<http::vector_body<char>>& req) = 0;
    protected:
        ServerPaths server_paths_;
        std::string file_path_;
        boost::beast::http::request_parser<boost::beast::http::string_body> parser;
        const char* data;
        short bytes_transferred;

        // std::vector<char> create_echo_response();
        // std::vector<char> create_static_response();

        // std::vector<char> generate_404_response();
        // std::vector<char> generate_500_response();

        // bool isTargetStatic();
        // bool isTargetEcho();
        // std::string get_response_content_type();

        // std::string determine_content_type(const std::string& file_path);
        // std::string resolve_to_physical_path(boost::beast::string_view target);


        

};
#endif