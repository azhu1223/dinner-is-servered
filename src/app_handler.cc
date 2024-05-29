#include "app_handler.h"
#include "logging_buffer.h"
#include <fstream>



AppHandler::AppHandler(LoggingBuffer* logging_buffer) : RequestHandler(), logging_buffer_(logging_buffer) {}


http::response<http::vector_body<char>> AppHandler::handle_request(const http::request<http::vector_body<char>>& req){
    return generate_landing_page ();
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
