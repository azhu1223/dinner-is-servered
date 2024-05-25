#include "sleep_handler.h"
#include "request_handler.h"
#include <string>
#include <boost/log/trivial.hpp>
#include <chrono>
#include <thread>

namespace http = boost::beast::http;

SleepHandler::SleepHandler(LoggingBuffer* logging_buffer) : RequestHandler(), logging_buffer_(logging_buffer) {}

http::response<http::vector_body<char>> SleepHandler::handle_request(const http::request<http::vector_body<char>>& req) {
    std::vector<char> response_body_vector;
    
    auto response = http::response<http::vector_body<char>>(http::status::ok, 11U, response_body_vector);

    response.set(http::field::content_type, "text/plain");
    response.set(http::field::content_length, std::to_string(response_body_vector.size()));

    response.prepare_payload();

    logging_buffer_->addToBuffer(INFO, "Going to sleep");
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    logging_buffer_->addToBuffer(INFO, "Now awake");

    return response;
}

RequestHandler* SleepHandlerFactory::create(LoggingBuffer* logging_buffer) {
    return new SleepHandler(logging_buffer);
}