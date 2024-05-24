#include "sleep_handler.h"
#include "request_handler.h"
#include <string>
#include <boost/log/trivial.hpp>
#include <chrono>
#include <thread>

namespace http = boost::beast::http;

SleepHandler::SleepHandler() : RequestHandler() {}

http::response<http::vector_body<char>> SleepHandler::handle_request(const http::request<http::vector_body<char>>& req) {
    std::vector<char> response_body_vector;
    
    auto response = http::response<http::vector_body<char>>(http::status::ok, 11U, response_body_vector);

    response.set(http::field::content_type, "text/plain");
    response.set(http::field::content_length, std::to_string(response_body_vector.size()));

    response.prepare_payload();

    BOOST_LOG_TRIVIAL(info) << "Going to sleep";
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    BOOST_LOG_TRIVIAL(info) << "Now awake";

    return response;
}

RequestHandler* SleepHandlerFactory::create() {
    return new SleepHandler();
}