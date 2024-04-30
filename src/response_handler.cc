#include "response_handler.h"
#include <string>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/beast/http/parser.hpp>
#include <boost/beast/http.hpp>
#include <boost/log/trivial.hpp>
#include <fstream>


ResponseHandler::ResponseHandler(short bytes_transferred, const char data[], ServerPaths server_paths)
    :server_paths_(server_paths), file_path_("")
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

// std::vector<char> ResponseHandler::create_response(){
//     BOOST_LOG_TRIVIAL(info) << "Determining request type";
//     auto message = this->parser.get();
//     boost::beast::http::verb method = message.method();
//     boost::beast::string_view target = message.target();


//     if(method == boost::beast::http::verb::get && this->isTargetStatic()){
//         BOOST_LOG_TRIVIAL(info) << "Request was static";
//         return this->create_static_response();
//     }
//     //Note: keep echo at the bottom since / will often be included in config file as an echo path
//     else if (method == boost::beast::http::verb::get && this->isTargetEcho()){
//         BOOST_LOG_TRIVIAL(info) << "Request was echo";
//         return this->create_echo_response();
//     }
//     else{
//         BOOST_LOG_TRIVIAL(error) << "Invalid request type";
//     }
//     return std::vector<char>();
// }

// std::vector<char> ResponseHandler::create_echo_response(){
//     //TODO - make response without using data and bytes_transfered. Thus, only requiring the parser member variable

//     const std::string status = "HTTP/1.0 200 OK\r\n";
//     const std::string content_type = "Content-Type: text/plain\r\n";
//     const std::string content_length_text = "Content-Length: " + std::to_string(bytes_transferred) + "\r\n\r\n";

//     // Constructs the header
//     const std::string header = status + content_type + content_length_text;

//     std::vector<char> res(header.begin(), header.end());

//     // Adds the GET request sent to the server as the contents of the reponse
//     for (int i=0; i < bytes_transferred; i++){
//         res.push_back(data[i]);
//     }

//     return res;
// }

// std::vector<char> ResponseHandler::create_static_response() {
//     // Check whether file is found
//     std::ifstream file(file_path_, std::ios::binary);
//     if (!file.is_open()) {
//         return this->generate_404_response();
//     }
//     BOOST_LOG_TRIVIAL(info) << "File found";

//     // Get file size
//     file.seekg(0, std::ios::end);
//     std::streampos file_size = file.tellg();
//     file.seekg(0, std::ios::beg);
//     BOOST_LOG_TRIVIAL(info) << "Obtained file size of " << file_size;

//     std::string response_content_type = this->get_response_content_type();
//     //Check if unknonw type
//     if (response_content_type == ""){
//         return this->generate_500_response();
//     }


//     // Serve file
//     const std::string status = "HTTP/1.0 200 OK\r\n";
//     const std::string content_type = "Content-Type: " + response_content_type +"\r\n";
//     const std::string content_length_text = "Content-Length: " + std::to_string(file_size) + "\r\n\r\n";
//     const std::string header = status + content_type + content_length_text;

//     // Allocate buffer for response
//     std::vector<char> res(header.size() + file_size);

//     // Copy header to the beginning of the response
//     std::copy(header.begin(), header.end(), res.begin());

//     // Read file content into the response buffer after the header
//     if (!file.read(res.data() + header.size(), file_size)) {
//         // Failed to read file
//         file.close();
//         BOOST_LOG_TRIVIAL(fatal) << "Failed to read file";
//         throw;
//     }

//     file.close();

//     return res;
// }

// std::vector<char> ResponseHandler::generate_404_response() {
//     BOOST_LOG_TRIVIAL(error) << "404 not found: ";
//     std::string response = "HTTP/1.1 404 Not Found\r\n"
//                            "Content-Type: text/plain\r\n"
//                            "Content-Length: 17\r\n\r\n"
//                            "404 Not Found\r\n\r\n";
//     return std::vector<char>(response.begin(), response.end());
// }

// std::vector<char> ResponseHandler::generate_500_response(){
//     BOOST_LOG_TRIVIAL(error) << "500 Internal Server Error: ";
//     std::string response = "HTTP/1.1 500 Internal Server Error\r\n"
//                            "Content-Type: text/plain\r\n"
//                            "Content-Length: 17\r\n\r\n"
//                            "500 Internal Server Error\r\n\r\n";
//     return std::vector<char>(response.begin(), response.end());
// }



// std::string ResponseHandler::get_response_content_type(){
//     size_t extension_pos = this->file_path_.find_last_of(".");
//     if (extension_pos == std::string::npos) {
//         BOOST_LOG_TRIVIAL(error) << "No file extension found";
//         return "";
//     }

//     std::string extension = this->file_path_.substr(extension_pos);

//     if(extension == ".jpeg" || extension == ".jpg"){
//         return "image/jpeg";
//     }
//     else if (extension == ".html"){
//         return "text/html";
//     }
//     else if (extension == ".txt"){
//         return "text/plain";
//     }
//     else if (extension == ".zip"){
//         return "application/zip";
//     }

//     BOOST_LOG_TRIVIAL(error) << "File extension type not supported";
//     return "";
// }

// bool ResponseHandler::isTargetStatic(){
//     auto message = this->parser.get();
//     boost::beast::string_view target = message.target();

//     std::string result;
//     bool is_prev_shash = false;

//     //Remove back to back slash
//     for (char c : target) {
//         if (c == '/' && is_prev_shash) {
//             continue;
//         }
//         result += c;
//         is_prev_shash = (c == '/');
//     }

//     for (auto path : server_paths_.static_){
//         if(result.find(path) == 0){
//             file_path_ = "../resources" + result.substr(result.find(path)+path.length());
//             BOOST_LOG_TRIVIAL(info) << "Set file_path_ to " << file_path_;
//             return true;
//         }
//     }
//     return false;

// }

// bool ResponseHandler::isTargetEcho(){
//     auto message = this->parser.get();
//     boost::beast::string_view target = message.target();

//     std::string result;
//     bool is_prev_shash = false;

//     //Remove back to back slash
//     for (char c : target) {
//         if (c == '/' && is_prev_shash) {
//             continue;
//         }
//         result += c;
//         is_prev_shash = (c == '/');
//     }

//     for (auto path : server_paths_.echo_){
//         if(result.find(path) == 0){
//             return true;
//         }
//     }
//     return false;

// }