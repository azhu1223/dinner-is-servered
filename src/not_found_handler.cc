#include "not_found_handler.h"
#include "request_handler.h"
#include <string>
#include <boost/log/trivial.hpp>

namespace http = boost::beast::http;

NotFoundHandler::NotFoundHandler() : RequestHandler() {}

http::response<http::vector_body<char>> NotFoundHandler::handle_request(const http::request<http::vector_body<char>>& req) {
    std::string response_body_string = "404 Not Found\r\n\r\n";
    std::vector<char> response_body_vector(response_body_string.begin(), response_body_string.end());

    auto response = http::response<http::vector_body<char>>(http::status::not_found, 11U, response_body_vector);

    response.set(http::field::content_type, "text/plain");
    response.set(http::field::content_length, std::to_string(response_body_vector.size()));

    response.prepare_payload(); 


    return response;
}

RequestHandler* NotFoundHandlerFactory::create() {
    return new NotFoundHandler();
}