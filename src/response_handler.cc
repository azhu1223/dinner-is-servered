#include "response_handler.h"
#include <string>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/beast/http/parser.hpp>
#include <boost/beast/http.hpp>
#include <boost/log/trivial.hpp>



ResponseHandler::ResponseHandler(short bytes_transferred, const char data[]){
    // Create a request parser
    // boost::beast::http::request_parser<boost::beast::http::string_body> parser;
    auto buffer = boost::asio::buffer(data, bytes_transferred);
    // Parse the buffer
    boost::beast::error_code ec;
    this->parser.put(buffer, ec);
    this->bytes_transferred = bytes_transferred;
    this->data = data;
    // this->msg = parser.get();

    //Get specific elements of the parsed response
    // this->method = parsed_request.method();
    // this->target = parsed_request.target();
}

std::vector<char> ResponseHandler::create_response(){
    BOOST_LOG_TRIVIAL(info) << "Determining request type";
    auto message = this->parser.get();
    boost::beast::http::verb method = message.method();
    boost::beast::string_view target = message.target();


    if(method == boost::beast::http::verb::get && this->isTargetStatic()){
        BOOST_LOG_TRIVIAL(info) << "Request was static";
        return this->create_static_response();
    }
    else if (method == boost::beast::http::verb::get && !this->isTargetStatic()){
        BOOST_LOG_TRIVIAL(info) << "Request was echo";
        return this->create_echo_response();
    }
    else{
        BOOST_LOG_TRIVIAL(error) << "Invalid request type";
    }
    return std::vector<char>();
}

std::vector<char> ResponseHandler::create_echo_response(){
    //TODO - make response without using data and bytes_transfered. Thus, only requiring the parser member variable

    const std::string status = "HTTP/1.0 200 OK\r\n";
    const std::string content_type = "Content-Type: text/plain\r\n";
    const std::string content_length_text = "Content-Length: " + std::to_string(bytes_transferred) + "\r\n\r\n";

    // Constructs the header
    const std::string header = status + content_type + content_length_text;

    std::vector<char> res(header.begin(), header.end());

    // Adds the GET request sent to the server as the contents of the reponse
    for (int i=0; i < bytes_transferred; i++){
        res.push_back(data[i]);
    }

    return res;
}

std::vector<char> ResponseHandler::create_static_response(){
    //TODO update to properly serve the static file
    const std::string header = "STATIC FILE";
    std::vector<char> res(header.begin(), header.end());
    return res; 
}

bool ResponseHandler::isTargetStatic(){
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

    //TODO- update to be configurable from config file
    return result.find("/static") == 0;

}