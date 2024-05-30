#include "app_handler.h"
#include "logging_buffer.h"
#include <fstream>
#include <chrono>
#include <thread>
#include <string>



AppHandler::AppHandler(LoggingBuffer* logging_buffer) : RequestHandler(), logging_buffer_(logging_buffer) {}


http::response<http::vector_body<char>> AppHandler::handle_request(const http::request<http::vector_body<char>>& req){
   
    if (req.method() == http::verb::post) {
      return process_post(req);
    } else if (req.method() == http::verb::get) {
      return generate_landding_page ();;
    } else {
        std::vector<char> body;
        http::response<http::vector_body<char>> response = http::response<http::vector_body<char>>(http::status::bad_request, 11U, body);
        response.prepare_payload();
        return response;
    }
    
}

http::response<http::vector_body<char>> AppHandler::process_post(const http::request<http::vector_body<char>>& req){
    
    std::string dummy_response_string = "Response Processed";
    std::vector<char> response_body_vector(dummy_response_string.begin(), dummy_response_string.end());
    
    auto response = http::response<http::vector_body<char>>(http::status::ok, 11U, response_body_vector);

    response.set(http::field::content_type, "text/plain");
    response.set(http::field::content_length, std::to_string(response_body_vector.size()));

    response.prepare_payload();

    logging_buffer_->addToBuffer(INFO, "Going to sleep");
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    logging_buffer_->addToBuffer(INFO, "Now awake");

    return response;
}


http::response<http::vector_body<char>> AppHandler::generate_landing_page(){

    http::response<http::vector_body<char>> response;

    std::string file_path = "../app_pages/index.html";

    std::ifstream file(file_path, std::ios::in | std::ios::binary);

    if (!file.is_open()) {
        logging_buffer_->addToBuffer(ERROR, "File not found");
        response = http::response<http::vector_body<char>>(http::status::not_found, 11U);
        response.prepare_payload();
        return response;
    }

    // Get file size
    file.seekg(0, std::ios::end);
    std::streampos file_size = file.tellg();
    file.seekg(0, std::ios::beg);
    logging_buffer_->addToBuffer(INFO, "Obtained file size of " + std::to_string(file_size));

    // Allocate buffer for file
    std::vector<char> res(file_size);

    // Read file content into the response buffer after the header
    if (!file.read(res.data(), file_size)) {
        // Failed to read file
        file.close();
        logging_buffer_->addToBuffer(ERROR, "Failed to read file");
    }

    response = http::response<http::vector_body<char>>(http::status::ok, 11U, res);
    response.set(http::field::content_type, "text/html");
    response.set(http::field::content_length, std::to_string(res.size()));

    response.prepare_payload(); 


    return response;
}


RequestHandler* AppHandlerFactory::create(LoggingBuffer* logging_buffer) {
    return new AppHandler(logging_buffer);
}
