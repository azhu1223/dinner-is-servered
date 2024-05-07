#ifndef STATIC_HANDLER_H
#define STATIC_HANDLER_H

#include "response_handler.h"
#include "utils.h"
#include <vector>
#include <string>

class StaticHandler : public ResponseHandler {
    public: 
        StaticHandler(short bytes_transferred, const char data[], ServerPaths server_paths, std::string file_path);
        virtual std::vector<char> create_response();
        std::vector<char> generate_404_response();
        std::vector<char> generate_500_response();
        std::string get_response_content_type();
        // std::string determine_content_type(const std::string& file_path);
        // std::string resolve_to_physical_path(boost::beast::string_view target);
    protected:
        std::string file_path_;
};

#endif