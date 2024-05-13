#ifndef STATIC_HANDLER_H
#define STATIC_HANDLER_H

#include "request_handler.h"
#include "utils.h"
#include <vector>
#include <string>

namespace http = boost::beast::http;

class StaticHandler : public RequestHandler {
    public: 
        StaticHandler();
        //StaticHandler(short bytes_transferred, const char data[], ServerPaths server_paths, std::string file_path);
        //virtual std::vector<char> create_response();
        virtual http::response<http::vector_body<char>> handle_request(const http::request<http::vector_body<char>>& req);
        //std::vector<char> generate_404_response();
        //std::vector<char> generate_500_response();
        std::string get_response_content_type(const std::string& file_path);
        // std::string determine_content_type(const std::string& file_path);
        // std::string resolve_to_physical_path(boost::beast::string_view target);
    protected:
        std::string file_path_;
};

#endif