#include "request_parser.h"
#include <boost/beast/http/parser.hpp>
#include <boost/log/trivial.hpp>
#include <boost/beast/http.hpp>
#include "config_interpreter.h"

RequestParser::RequestParser(short bytes_transferred, const char data[], ServerPaths server_paths) : server_paths_(server_paths), file_path_("") {
    auto buffer = boost::asio::buffer(data, bytes_transferred);
    boost::beast::error_code ec;
    this->parser.put(buffer, ec);
    this->request_type = RequestType::None;
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

    for (auto path : server_paths_.static_){
        if(result.find(path) == 0){
            file_path_ = "../resources" + result.substr(result.find(path)+path.length());
            BOOST_LOG_TRIVIAL(info) << "Set file_path_ to " << file_path_;
            return true;
        }
    }
    return false;
}