#include "not_found_handler.h"
#include "request_handler.h"
#include <string>
#include <boost/log/trivial.hpp>

namespace http = boost::beast::http;

NotFoundHandler::NotFoundHandler() : RequestHandler() {}

NotFoundHandler::NotFoundHandler(short bytes_transferred, const char data[], ServerPaths server_paths) : RequestHandler(bytes_transferred, data, server_paths) {}

std::vector<char> NotFoundHandler::create_response() {
    BOOST_LOG_TRIVIAL(error) << "404 not found: ";
    std::string response = "HTTP/1.1 404 Not Found\r\n"
                           "Content-Type: text/plain\r\n"
                           "Content-Length: 17\r\n\r\n"
                           "404 Not Found\r\n\r\n";
    return std::vector<char>(response.begin(), response.end());
}

http::response<http::vector_body<char>> NotFoundHandler::handle_request(const http::request<http::vector_body<char>>& req) {
    std::string response_body_string = "404 Not Found\r\n\r\n";
    std::vector<char> response_body_vector(response_body_string.begin(), response_body_string.end());

    auto response = http::response<http::vector_body<char>>(http::status::not_found, 11U, response_body_vector);

    response.set(http::field::content_type, "text/plain");
    response.set(http::field::content_length, std::to_string(response_body_vector.size()));

    return response;
}

