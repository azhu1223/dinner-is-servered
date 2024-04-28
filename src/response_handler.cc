#include "response_handler.h"
#include <string>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/beast/http/parser.hpp>
#include <boost/beast/http.hpp>
#include <boost/log/trivial.hpp>
#include <fstream>
#include <sstream>
#include <filesystem> 

ResponseHandler::ResponseHandler(short bytes_transferred, const char data[], ServerPaths server_paths)
    :server_paths_(server_paths)
{
    // Create a request parser
    // boost::beast::http::request_parser<boost::beast::http::string_body> parser;
    auto buffer = boost::asio::buffer(data, bytes_transferred);
    // Parse the buffer
    boost::beast::error_code ec;
    this->parser.put(buffer, ec);
    this->bytes_transferred = bytes_transferred;
    this->data = data;
    // this->msg = parser.get();

    //Get specific elements of the parsed response
    // this->method = parsed_request.method();
    // this->target = parsed_request.target();
}

std::vector<char> ResponseHandler::create_response(){
    BOOST_LOG_TRIVIAL(info) << "Determining request type";
    auto message = this->parser.get();
    boost::beast::http::verb method = message.method();
    boost::beast::string_view target = message.target();


    if(method == boost::beast::http::verb::get && this->isTargetStatic()){
        BOOST_LOG_TRIVIAL(info) << "Request was static";
        return this->create_static_response();
    }
    //Note: keep echo at the bottom since / will often be included in config file as an echo path
    else if (method == boost::beast::http::verb::get && this->isTargetEcho()){
        BOOST_LOG_TRIVIAL(info) << "Request was echo";
        return this->create_echo_response();
    }
    else{
        BOOST_LOG_TRIVIAL(error) << "Invalid request type";
    }
    return std::vector<char>();
}

std::vector<char> ResponseHandler::create_echo_response(){
    //TODO - make response without using data and bytes_transfered. Thus, only requiring the parser member variable

    const std::string status = "HTTP/1.0 200 OK\r\n";
    const std::string content_type = "Content-Type: text/plain\r\n";
    const std::string content_length_text = "Content-Length: " + std::to_string(bytes_transferred) + "\r\n\r\n";

    // Constructs the header
    const std::string header = status + content_type + content_length_text;

    std::vector<char> res(header.begin(), header.end());

    // Adds the GET request sent to the server as the contents of the reponse
    for (int i=0; i < bytes_transferred; i++){
        res.push_back(data[i]);
    }

    return res;
}

std::vector<char> ResponseHandler::create_static_response() {
    auto message = this->parser.get();
    boost::beast::string_view target = message.target();
    std::string file_path = resolve_to_physical_path(target);

    std::ifstream file(file_path, std::ios::binary | std::ios::ate);
    if (!file) {
        BOOST_LOG_TRIVIAL(error) << "Failed to open file: " << file_path;  // Log file opening failure
        return generate_404_response();  // implemented this function to generate a 404 response
    }

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<char> buffer(size);
    if (!file.read(buffer.data(), size)) {
        BOOST_LOG_TRIVIAL(error) << "Failed to read file: " << file_path;  // Log file reading failure
        return generate_500_response();  // implemented this function to generate a 500 response
    }

    // Create the response headers
    std::stringstream ss;
    ss << "HTTP/1.1 200 OK\r\nContent-Type: " << determine_content_type(file_path) << "\r\nContent-Length: " << size << "\r\n\r\n";
    std::string header = ss.str();
    std::vector<char> response(header.begin(), header.end());
    response.insert(response.end(), buffer.begin(), buffer.end());

    return response;
}

std::string ResponseHandler::determine_content_type(const std::string& file_path) {
    const size_t pos = file_path.rfind('.');
    if (pos != std::string::npos) {
        std::string extension = file_path.substr(pos);
        if (extension == ".html") return "text/html";
        else if (extension == ".js") return "application/javascript";
        else if (extension == ".css") return "text/css";
        else if (extension == ".png") return "image/png";
        else if (extension == ".jpg" || extension == ".jpeg") return "image/jpeg";
    }
    return "application/octet-stream";  // Default MIME type for unknown files
}


std::string ResponseHandler::resolve_to_physical_path(boost::beast::string_view target) {
    try {
        std::filesystem::path base_path = std::filesystem::canonical("/your/base/directory");
        std::filesystem::path requested_path = std::filesystem::canonical(base_path / std::filesystem::path(target.to_string()));

        if (requested_path.string().substr(0, base_path.string().length()) != base_path.string()) {
            BOOST_LOG_TRIVIAL(error) << "Directory traversal attempt detected: " << target;
            throw std::runtime_error("Access denied");
        }

        if (!std::filesystem::exists(requested_path) || std::filesystem::is_directory(requested_path)) {
            BOOST_LOG_TRIVIAL(error) << "File not found or is a directory: " << requested_path;
            throw std::runtime_error("File not found");
        }

        return requested_path.string();
    } catch (const std::filesystem::filesystem_error& e) {
        BOOST_LOG_TRIVIAL(error) << "Filesystem error: " << e.what();
        throw;
    }
}


std::vector<char> ResponseHandler::generate_404_response() {
    std::string response = "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\n\r\n<html><body><h1>404 Not Found</h1><p>The requested resource was not found on this server.</p></body></html>";
    return std::vector<char>(response.begin(), response.end());
}

std::vector<char> ResponseHandler::generate_500_response() {
    std::string response = "HTTP/1.1 500 Internal Server Error\r\nContent-Type: text/html\r\n\r\n<html><body><h1>500 Internal Server Error</h1><p>An error occurred while processing your request.</p></body></html>";
    return std::vector<char>(response.begin(), response.end());
}



bool ResponseHandler::isTargetStatic(){
    auto message = this->parser.get();
    boost::beast::string_view target = message.target();

    std::string result;
    bool is_prev_shash = false;

    //Remove back to back slash
    for (char c : target) {
        if (c == '/' && is_prev_shash) {
            continue;
        }
        result += c;
        is_prev_shash = (c == '/');
    }

    for (auto path : server_paths_.static_){
        if(result.find(path) == 0){
            return true;
        }
    }
    return false;

}

bool ResponseHandler::isTargetEcho(){
    auto message = this->parser.get();
    boost::beast::string_view target = message.target();

    std::string result;
    bool is_prev_shash = false;

    //Remove back to back slash
    for (char c : target) {
        if (c == '/' && is_prev_shash) {
            continue;
        }
        result += c;
        is_prev_shash = (c == '/');
    }

    for (auto path : server_paths_.echo_){
        if(result.find(path) == 0){
            return true;
        }
    }
    return false;

}