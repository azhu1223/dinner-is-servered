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
    this->setRequestType();
}

void RequestParser::setRequestType() {
    auto message = this->parser.get();
    boost::beast::string_view target = message.target();

    std::string result;
    bool is_prev_shash = false;
    RequestType request_type = None;

    //Remove back to back slash
    for (char c : target) {
        if (c == '/' && is_prev_shash) {
            continue;
        }
        result += c;
        is_prev_shash = (c == '/');
    }


    //Look for the closest matching path
    int longsest_size = 0;
    std::string longest_path = "";
    for (auto path_to_location : server_paths_.static_){
        std::string path = path_to_location.first;
        if(result.find(path) == 0 && path.length() > longsest_size){
            longsest_size = path.length();
            longest_path = path;
            request_type = Static;
        }
    }
    for (auto path : server_paths_.echo_){
        if(result.find(path) == 0 && path.length() > longsest_size){
            longsest_size = path.length();
            longest_path = path;
            request_type = Echo;
        }
    }

    if (request_type == None){
        BOOST_LOG_TRIVIAL(error) << "There was no mathcing path found for the request";
        this->request_type_ = request_type;
        return;
    }


    BOOST_LOG_TRIVIAL(info) << "Request is of type " << request_type;
    this->request_type_ = request_type;

    if (request_type == Static){
        this->file_path_ = "../resources" + server_paths_.static_[longest_path] 
                        + result.substr(result.find(longest_path) + longest_path.length());
        BOOST_LOG_TRIVIAL(info) << "Set file_path_ to " << file_path_;
    }

}

std::string RequestParser::getFilePath() {
    return this->file_path_;
}

boost::beast::string_view RequestParser::getTarget(){
    return this->parser.get().target();
}

RequestType RequestParser::getRequestType(){
    return this->request_type_;
}