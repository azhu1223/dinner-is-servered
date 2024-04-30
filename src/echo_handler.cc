#include "echo_handler.h"
#include <string>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/beast/http/parser.hpp>
#include <boost/beast/http.hpp>
#include <boost/log/trivial.hpp>
#include "response_handler.h"

EchoHandler::EchoHandler(short bytes_transferred, const char data[], ServerPaths server_paths) : ResponseHandler(bytes_transferred,data,server_paths) {}

std::vector<char> EchoHandler::create_response() {
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
