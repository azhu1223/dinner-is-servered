#include "app_handler.h"
#include "logging_buffer.h"


AppHandler::AppHandler(LoggingBuffer* logging_buffer) : RequestHandler(), logging_buffer_(logging_buffer) {}


http::response<http::vector_body<char>> AppHandler::handle_request(const http::request<http::vector_body<char>>& req){
    return generate_landing_page ();
}


http::response<http::vector_body<char>> AppHandler::generate_landing_page(){
    std::vector<char> response_body_vector;
    
    auto response = http::response<http::vector_body<char>>(http::status::ok, 11U, response_body_vector);

    response.set(http::field::content_type, "text/plain");
    response.set(http::field::content_length, std::to_string(response_body_vector.size()));

    response.prepare_payload(); 


    return response;
}


RequestHandler* AppHandlerFactory::create(LoggingBuffer* logging_buffer) {
    return new AppHandler(logging_buffer);
}
