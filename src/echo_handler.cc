#include "echo_handler.h"
#include "request_handler.h"
#include <string>
#include <vector>

namespace http = boost::beast::http;


EchoHandler::EchoHandler() : RequestHandler() {}

http::response<http::vector_body<char>> EchoHandler::handle_request(const http::request<http::vector_body<char>>& req) {
    auto req_base = req.base();
    std::string request_method_string = req_base.method_string().to_string();
    std::string request_target = req_base.target().to_string();

    unsigned int request_version = req.version();
    unsigned int request_major_version = request_version / 10;
    unsigned int request_minor_version = request_version % 10;

    // Should result in "GET {TARGET} HTTP/1.1\r\n"
    std::string request_header_string = request_method_string + ' ' + request_target + ' ' + "HTTP/" + std::to_string(request_major_version) + '.' + std::to_string(request_minor_version) + "\r\n"; 

    // Add the request fields to the request header string
    auto field_end = req_base.cend();
    for (auto field = req_base.cbegin(); field != field_end; field++) {
        std::string field_name = field->name_string().to_string();
        std::string field_value = field->value().to_string();

        // "{field_name}: {field_value}\r\n"
        request_header_string += field_name + ": " + field_value + "\r\n";
    }

    request_header_string += "\r\n";

    std::vector<char> request_body = req.body();

    // Start the response body with the request header
    std::vector<char> response_body(request_header_string.begin(), request_header_string.end());

    // Add the request body after the header
    response_body.insert(response_body.end(), request_body.begin(), request_body.end());

    http::response<http::vector_body<char>> response(http::status::ok, request_version, response_body);

    int content_length = response_body.size();
    response.set(http::field::content_type, "text/plain");
    response.set(http::field::content_length, std::to_string(content_length));

    return response;
}


RequestHandler* EchoHandlerFactory::create() {
    return new EchoHandler();
}