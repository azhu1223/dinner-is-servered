#include "health_handler.h"
#include "request_handler.h"
#include "logging_buffer.h"
#include <string>
#include <boost/log/trivial.hpp>

namespace http = boost::beast::http;

HealthHandler::HealthHandler(LoggingBuffer* logging_buffer) : RequestHandler(), logging_buffer_(logging_buffer) {}

http::response<http::vector_body<char>> HealthHandler::handle_request(const http::request<http::vector_body<char>>& req) {
    std::vector<char> response_body_vector;
    
    auto response = http::response<http::vector_body<char>>(http::status::ok, 11U, response_body_vector);

    response.set(http::field::content_type, "text/plain");
    response.set(http::field::content_length, std::to_string(response_body_vector.size()));

    response.prepare_payload(); 


    return response;
}

RequestHandler* HealthHandlerFactory::create(LoggingBuffer* logging_buffer) {
    return new HealthHandler(logging_buffer);
}