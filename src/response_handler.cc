#include "response_handler.h"
#include <string>
#include <iostream>

std::vector<char> create_response(short bytes_transferred, const char data[]) {
    const std::string status = "HTTP/1.0 200 OK\r\n";
    const std::string content_type = "Content-Type: text/plain\r\n";
    const std::string content_length_text = "Content-Length: " + std::to_string(bytes_transferred) + "\r\n\r\n";

    // Constructs the header
    const std::string header = status + content_type + content_length_text;

    std::vector<char> response(header.begin(), header.end());

    // Adds the GET request sent to the server as the contents of the reponse
    for (int i=0; i < bytes_transferred; i++){
        response.push_back(data[i]);
    }

    return response;
}
