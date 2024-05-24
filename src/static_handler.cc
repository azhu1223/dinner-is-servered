#include "request_handler.h"
#include "static_handler.h"
#include "utils.h"
#include <boost/log/trivial.hpp>
#include <fstream>
#include <vector>
#include <string>
#include "request_dispatcher.h"

StaticHandler::StaticHandler() : RequestHandler() {}

std::string StaticHandler::get_response_content_type(const std::string& file_path) {
    size_t extension_pos = file_path.find_last_of(".");
    if (extension_pos == std::string::npos) {
        BOOST_LOG_TRIVIAL(error) << "No file extension found";
        return "";
    }

    std::string extension = file_path.substr(extension_pos);

    if(extension == ".jpeg" || extension == ".jpg"){
        return "image/jpeg";
    }
    else if (extension == ".html"){
        return "text/html";
    }
    else if (extension == ".txt"){
        return "text/plain";
    }
    else if (extension == ".zip"){
        return "application/zip";
    }

    BOOST_LOG_TRIVIAL(error) << "File extension type not supported";
    return "";
}

http::response<http::vector_body<char>> StaticHandler::handle_request(const http::request<http::vector_body<char>>& req) {
    http::response<http::vector_body<char>> response;
    //Check that request is a GET
    if (req.method() != boost::beast::http::verb::get){
        BOOST_LOG_TRIVIAL(error) << "Invalid HTTP method, expected GET";
        std::string response_body_string = "Invalid HTTP method, expected GET\r\n\r\n";
        std::vector<char> body(response_body_string.begin(), response_body_string.end());
        response.set(http::field::content_type, "text/plain");
        response.set(http::field::content_length, std::to_string(body.size()));
        response = http::response<http::vector_body<char>>(http::status::bad_request, 11U, body);
        response.prepare_payload();
        return response;
    }


    std::string file_path = RequestDispatcher::getStaticFilePath(req);

    std::ifstream file(file_path, std::ios::in | std::ios::binary);

    if (!file.is_open()) {
        BOOST_LOG_TRIVIAL(error) << "File not found";
        response = http::response<http::vector_body<char>>(http::status::not_found, 11U);
        response.prepare_payload();
        return response;
    }
    
    BOOST_LOG_TRIVIAL(info) << "File found";

    // Get file size
    file.seekg(0, std::ios::end);
    std::streampos file_size = file.tellg();
    file.seekg(0, std::ios::beg);
    BOOST_LOG_TRIVIAL(info) << "Obtained file size of " << file_size;

    std::string response_content_type = this->get_response_content_type(file_path);

    if (response_content_type.empty()) {
        BOOST_LOG_TRIVIAL(error) << "Unsupported file type";
        response = http::response<http::vector_body<char>>(http::status::not_found, 11U);   //Will also occur when trying to access a directory on a static serving path
        response.prepare_payload();
        return response;
    }

    // Allocate buffer for file
    std::vector<char> res(file_size);

    // Read file content into the response buffer after the header
    if (!file.read(res.data(), file_size)) {
        // Failed to read file
        file.close();
        BOOST_LOG_TRIVIAL(fatal) << "Failed to read file";
    }

    file.close();

    response = http::response<http::vector_body<char>>(http::status::ok, 11U, res);
    response.set(http::field::content_type, response_content_type);
    response.set(http::field::content_length, std::to_string(res.size()));

    return response;
}

RequestHandler* StaticHandlerFactory::create() {
    return new StaticHandler();
}