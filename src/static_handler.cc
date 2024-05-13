#include "request_handler.h"
#include "static_handler.h"
#include "utils.h"
#include <boost/log/trivial.hpp>
#include <fstream>
#include <vector>
#include <string>

StaticHandler::StaticHandler() : RequestHandler() {}

//StaticHandler::StaticHandler(short bytes_transferred, const char data[], ServerPaths server_paths, std::string file_path) : RequestHandler(bytes_transferred, data, server_paths), file_path_(file_path) {}

/*std::vector<char> StaticHandler::create_response() {
 // Check whether file is found
    std::ifstream file(file_path_, std::ios::binary);
    if (!file.is_open()) {
        return this->generate_404_response();
    }
    BOOST_LOG_TRIVIAL(info) << "File found";

    // Get file size
    file.seekg(0, std::ios::end);
    std::streampos file_size = file.tellg();
    file.seekg(0, std::ios::beg);
    BOOST_LOG_TRIVIAL(info) << "Obtained file size of " << file_size;

    std::string response_content_type = this->get_response_content_type(this->file_path_);
    //Check if unknonw type
    if (response_content_type == ""){
        return this->generate_500_response();
    }


    // Serve file
    const std::string status = "HTTP/1.0 200 OK\r\n";
    const std::string content_type = "Content-Type: " + response_content_type +"\r\n";
    const std::string content_length_text = "Content-Length: " + std::to_string(file_size) + "\r\n\r\n";
    const std::string header = status + content_type + content_length_text;

    // Allocate buffer for response
    std::vector<char> res(header.size() + file_size);

    // Copy header to the beginning of the response
    std::copy(header.begin(), header.end(), res.begin());

    // Read file content into the response buffer after the header
    if (!file.read(res.data() + header.size(), file_size)) {
        // Failed to read file
        file.close();
        BOOST_LOG_TRIVIAL(fatal) << "Failed to read file";
        throw;
    }

    file.close();

    return res;
}

std::vector<char> StaticHandler::generate_404_response() {
    BOOST_LOG_TRIVIAL(error) << "404 not found: ";
    std::string response = "HTTP/1.1 404 Not Found\r\n"
                           "Content-Type: text/plain\r\n"
                           "Content-Length: 17\r\n\r\n"
                           "404 Not Found\r\n\r\n";
    return std::vector<char>(response.begin(), response.end());
}

std::vector<char> StaticHandler::generate_500_response() {
    BOOST_LOG_TRIVIAL(error) << "500 Internal Server Error: ";
    std::string response = "HTTP/1.1 500 Internal Server Error\r\n"
                           "Content-Type: text/plain\r\n"
                           "Content-Length: 17\r\n\r\n"
                           "500 Internal Server Error\r\n\r\n";
    return std::vector<char>(response.begin(), response.end());
}*/

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

    BOOST_LOG_TRIVIAL(info) << "The target file is " << req.target().to_string();

    std::ifstream file(req.target().to_string(), std::ios::in | std::ios::binary);

    if (!file.is_open()) {
        BOOST_LOG_TRIVIAL(error) << "File not found";
        response = http::response<http::vector_body<char>>(http::status::not_found, 11U);
        return response;
    }
    
    BOOST_LOG_TRIVIAL(info) << "File found";

    // Get file size
    file.seekg(0, std::ios::end);
    std::streampos file_size = file.tellg();
    file.seekg(0, std::ios::beg);
    BOOST_LOG_TRIVIAL(info) << "Obtained file size of " << file_size;

    std::string response_content_type = this->get_response_content_type(req.target().to_string());

    if (response_content_type.empty()) {
        BOOST_LOG_TRIVIAL(error) << "Unsupported file type";
        response = http::response<http::vector_body<char>>(http::status::internal_server_error, 11U);
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

// std::string StaticHandler::determine_content_type(const std::string& file_path) {

// }

// std::string StaticHandler::resolve_to_physical_path(boost::beast::string_view target) {

// }