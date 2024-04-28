#ifndef RESPONSE_HANDLER_H
#define RESPONSE_HANDLER_H

#include <vector>
#include <boost/beast/http/parser.hpp>
#include <boost/beast/http.hpp>
#include "config_interpreter.h"
#include <boost/beast/core.hpp> 



class ResponseHandler{
    public:
        ResponseHandler(short bytes_transferred, const char data[], ServerPaths server_paths);
        std::vector<char> create_response();
    private:
        ServerPaths server_paths_;
        std::string file_path_;
        boost::beast::http::request_parser<boost::beast::http::string_body> parser;
        const char* data;
        short bytes_transferred;
        std::vector<char> create_echo_response();
        std::vector<char> create_static_response();

        std::vector<char> generate_404_response();
        std::vector<char> generate_500_response();

        bool isTargetStatic();
        bool isTargetEcho();
        std::string get_response_content_type();

        std::string determine_content_type(const std::string& file_path);
        std::string resolve_to_physical_path(boost::beast::string_view target);


        

};
#endif