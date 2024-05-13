#include "request_parser.h"
#include "utils.h"
#include <boost/beast/http/parser.hpp>
#include <boost/beast/http.hpp>
#include <boost/log/trivial.hpp>
#include <filesystem>

// RequestParser::RequestParser(short bytes_transferred, const char data[], ServerPaths server_paths) 
// : server_paths_(server_paths), file_path_("") {
//     auto buffer = boost::asio::buffer(data, bytes_transferred);
//     boost::beast::error_code ec;
//     this->parser.put(buffer, ec);
//     this->setRequestType();
// }

RequestParser::RequestParser(http::request<http::vector_body<char>> request, ServerPaths server_paths) : request_(request), target_(request_.target()), server_paths_(server_paths)
{
    this->setRequestType();
}

void RequestParser::setRequestType() {
    std::string result;
    bool is_prev_shash = false;
    RequestType request_type = None;

    //Remove back to back slashes and trailing slashes
    for (char c : this->target_) {
        if (c == '/' && is_prev_shash) {
            BOOST_LOG_TRIVIAL(info) << "Found multiple slashes in a row";
            continue;
        }
        result += c;
        is_prev_shash = (c == '/');
    }
    while (result.back() == '/' && result.length() > 1) {
        BOOST_LOG_TRIVIAL(info) << "Found trailing slash";
        result.pop_back();
    }

    //Look for the closest matching path
    int longest_size = 0;
    std::string longest_path = "";
    for (auto path_to_location : server_paths_.static_){
        std::string path = path_to_location.first;

        if(result.find(path) == 0 && path.length() > longest_size){
            longest_size = path.length();
            longest_path = path;
            request_type = Static;
        }
    }
    for (auto path : server_paths_.echo_){
        if(result.find(path) == 0 && path.length() > longest_size){
            longest_size = path.length();
            longest_path = path;
            request_type = Echo;
        }
    }

    if (request_type == None){
        BOOST_LOG_TRIVIAL(error) << "There was no matching path found for the request";
        this->request_type_ = request_type;
        return;
    }


    BOOST_LOG_TRIVIAL(info) << "Request is of type " << request_type;
    this->request_type_ = request_type;

    if (request_type == Static){
        this->file_path_ = server_paths_.static_[longest_path] 
                        + result.substr(result.find(longest_path) + longest_path.length());
        BOOST_LOG_TRIVIAL(info) << "Set file_path_ to " << file_path_;

        auto path = std::filesystem::path(file_path_);

        if (!(std::filesystem::exists(path) && std::filesystem::is_regular_file(path))) {
            BOOST_LOG_TRIVIAL(error) << "File at " << file_path_ <<  " not found";
            request_type = None;
            this->request_type_ = request_type;
        }
    }
}

std::string RequestParser::getFilePath() {
    return this->file_path_;
}

boost::beast::string_view RequestParser::getTarget(){
    return this->target_;
}

RequestType RequestParser::getRequestType(){
    return this->request_type_;
}