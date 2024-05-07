#include "request_parser.h"
#include "utils.h"
#include <boost/beast/http/parser.hpp>
#include <boost/beast/http.hpp>
#include <boost/log/trivial.hpp>

RequestParser::RequestParser(short bytes_transferred, const char data[], ServerPaths server_paths) 
: server_paths_(server_paths), file_path_("") {
    auto buffer = boost::asio::buffer(data, bytes_transferred);
    boost::beast::error_code ec;
    this->parser.put(buffer, ec);
}

bool RequestParser::isRequestEcho() {
    auto message = this->parser.get();
    boost::beast::string_view target = message.target();

    std::string result;
    bool is_prev_shash = false;

    //Remove back to back slash
    for (char c : target) {
        if (c == '/' && is_prev_shash) {
            continue;
        }
        result += c;
        is_prev_shash = (c == '/');
    }

    for (auto path : server_paths_.echo_){
        if(result.find(path) == 0){
            return true;
        }
    }
    return false;
}

bool RequestParser::isRequestStatic() {
    auto message = this->parser.get();
    boost::beast::string_view target = message.target();

    std::string result;
    bool is_prev_shash = false;

    //Remove back to back slash
    for (char c : target) {
        if (c == '/' && is_prev_shash) {
            continue;
        }
        result += c;
        is_prev_shash = (c == '/');
    }

    //Look for the closest matching static path
    int longsest_size = 0;
    std::string longest_path = "";
    for (auto path_to_location : server_paths_.static_){
        std::string path = path_to_location.first;
        if(result.find(path) == 0 && path.length() > longsest_size){
         longsest_size = path.length();
            longest_path = path;
        }
    }

    //Set location
    if(result.find(longest_path) == 0 && longest_path.length() > 0){
        file_path_ = "../resources" + server_paths_.static_[longest_path] 
                        + result.substr(result.find(longest_path) + longest_path.length());
        BOOST_LOG_TRIVIAL(info) << "Set file_path_ to " << file_path_;
        return true;
    }

    return false;
}

std::string RequestParser::getFilePath() {
    return this->file_path_;
}

boost::beast::string_view RequestParser::getTarget(){
    return this->parser.get().target();
}
