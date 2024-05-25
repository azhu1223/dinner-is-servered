#include "request_dispatcher.h"
#include "config_interpreter.h"
#include "utils.h"
#include <boost/beast/http/parser.hpp>
#include <boost/beast/http.hpp>
#include <boost/log/trivial.hpp>
#include <fstream>
#include "crud_file_manager.h"
#include "request_handler.h"
#include "not_found_handler.h"
#include "echo_handler.h"
#include "static_handler.h"
#include "registry.h"
#include "logging_buffer.h"

namespace http = boost::beast::http;


RequestType RequestDispatcher::getRequestType(const http::request<http::vector_body<char>>& request, LoggingBuffer* logging_buffer){
    boost::beast::string_view target = request.target();
    logging_buffer->addToBuffer(INFO, "Paths are: ");
    ServerPaths server_paths = ConfigInterpreter::getServerPaths();
    logging_buffer->addToBuffer(INFO, "Determining request type. Target is: " + target.to_string());
    logging_buffer->addToBuffer(INFO, "Paths are: ");
    for (auto path : server_paths.static_){
        logging_buffer->addToBuffer(INFO, path.first);
    }
    for (auto path : server_paths.echo_){
        logging_buffer->addToBuffer(INFO, path);
    }
    for (auto path : server_paths.crud_){
        logging_buffer->addToBuffer(INFO, path.first);
    }



    std::string result;
    bool is_prev_shash = false;
    RequestType request_type = None;

    //Remove back to back slashes and trailing slashes
    for (char c : target) {
        if (c == '/' && is_prev_shash) {
            logging_buffer->addToBuffer(INFO, "Found multiple slashes in a row");
            continue;
        }
        result += c;
        is_prev_shash = (c == '/');
    }
    while (result.back() == '/' && result.length() > 1) {
        logging_buffer->addToBuffer(INFO, "Found trailing slash");
        result.pop_back();
    }

    //Look for the closest matching path
    int longest_size = 0;
    std::string longest_path = "";
    for (auto path_to_location : server_paths.static_){
        std::string path = path_to_location.first;

        if(result.find(path) == 0 && path.length() > longest_size){
            longest_size = path.length();
            longest_path = path;
            request_type = Static;
        }
    }
    for (auto path_to_location : server_paths.crud_){
        std::string path = path_to_location.first;

        if(result.find(path) == 0 && path.length() > longest_size){
            longest_size = path.length();
            longest_path = path;
            request_type = CRUD;
        }
    }
    for (auto path : server_paths.echo_){
        if(result.find(path) == 0 && path.length() > longest_size){
            longest_size = path.length();
            longest_path = path;
            request_type = Echo;
        }
    }
    for (auto path : server_paths.health_){
        if(result.find(path) == 0 && path.length() > longest_size){
            longest_size = path.length();
            longest_path = path;
            request_type = Health;
        }
    }
    for (auto path : server_paths.sleep_){
        if(result.find(path) == 0 && path.length() > longest_size){
            longest_size = path.length();
            longest_path = path;
            request_type = Sleep;
        }
    }

    if (request_type == None){
        logging_buffer->addToBuffer(ERROR, "There was no matching path found for the request. 404 Handler should be invoked");
        return request_type;
    }
    else if (request_type == Static){
        std::ifstream file(server_paths.static_[longest_path] + result.substr(result.find(longest_path) + longest_path.length()),
            std::ios::in | std::ios::binary);
        if (!file.is_open()) {
            logging_buffer->addToBuffer(ERROR, "File not found. 404 Handler should be invoked");
            request_type = None;
            return request_type;
        }
    }


    logging_buffer->addToBuffer(INFO, "Request is of type " + RequestDispatcher::request_type_tostr(request_type, logging_buffer));
    return request_type;
}

std::string RequestDispatcher::getStaticFilePath
    (const http::request<http::vector_body<char>>& request, LoggingBuffer* logging_buffer){

    boost::beast::string_view target = request.target();
    ServerPaths server_paths = ConfigInterpreter::getServerPaths();


    std::string result;
    bool is_prev_shash = false;
    RequestType request_type = None;

    //Remove back to back slashes and trailing slashes
    for (char c : target) {
        if (c == '/' && is_prev_shash) {
            continue;
        }
        result += c;
        is_prev_shash = (c == '/');
    }
    while (result.back() == '/' && result.length() > 1) {
        result.pop_back();
    }



    //Look for the closest matching path
    int longest_size = 0;
    std::string longest_path = "";
    for (auto path_to_location : server_paths.static_){
        std::string path = path_to_location.first;
        if(result.find(path) == 0 && path.length() > longest_size){
            longest_size = path.length();
            longest_path = path;
            request_type = Static;
        }
    }
    for (auto path : server_paths.echo_){
        if(result.find(path) == 0 && path.length() > longest_size){
            longest_size = path.length();
            longest_path = path;
            request_type = Echo;
        }
    }

    if (request_type != Static){
        logging_buffer->addToBuffer(ERROR, "getStaticFilePath was called for a non-static request");
        return "";
    }


    std::string file_path = server_paths.static_[longest_path] 
                        + result.substr(result.find(longest_path) + longest_path.length());
    logging_buffer->addToBuffer(INFO, "Set file_path_ to " + file_path);
    return file_path;
};

CrudPath RequestDispatcher::getCrudEntityPath
    (const http::request<http::vector_body<char>>& request, LoggingBuffer* logging_buffer){

    boost::beast::string_view target = request.target();
    ServerPaths server_paths = ConfigInterpreter::getServerPaths();

    std::string result;
    bool is_prev_slash = false;
    RequestType request_type = None;

    //Remove back to back slashes and trailing slashes
    for (char c : target) {
        if (c == '/' && is_prev_slash) {
            continue;
        }
        result += c;
        is_prev_slash = (c == '/');
    }
    while (result.back() == '/' && result.length() > 1) {
        result.pop_back();
    }

    //Look for the closest matching path
    int longest_size = 0;
    std::string longest_path = "";
    for (auto path_to_location : server_paths.static_){
        std::string path = path_to_location.first;
        if(result.find(path) == 0 && path.length() > longest_size){
            longest_size = path.length();
            longest_path = path;
            request_type = Static;
        }
    }
    for (auto path : server_paths.echo_){
        if(result.find(path) == 0 && path.length() > longest_size){
            longest_size = path.length();
            longest_path = path;
            request_type = Echo;
        }
    }
    for (auto path_to_location : server_paths.crud_){
        std::string path = path_to_location.first;
        if(result.find(path) == 0 && path.length() > longest_size){
            longest_size = path.length();
            longest_path = path;
            request_type = CRUD;
        }
    }

    if (request_type != CRUD){
        logging_buffer->addToBuffer(ERROR, "getCrudEntityID was called for a non-CRUD request");
        CrudPath empty;
        return empty;
    }

    std::string entity_path = result.substr(result.find(longest_path) + longest_path.length());
     
    auto split_index = entity_path.find("/", 1);
    std::string entity_name = "";
    std::string entity_id = "";
    if (split_index != std::string::npos) {
      entity_name = entity_path.substr(1,split_index-1);
      entity_id = entity_path.substr(split_index+1, entity_path.size());
    } else if (entity_path.size() > 1) {
      entity_name = entity_path.substr(1);
    }
    logging_buffer->addToBuffer(INFO, "entity: " + entity_name);
    logging_buffer->addToBuffer(INFO, "id: " + entity_id);

    CrudPath crud_path;
    crud_path.data_path = server_paths.crud_[longest_path];
    crud_path.entity_name = entity_name;
    crud_path.entity_id = entity_id;

    return crud_path;
};

std::string RequestDispatcher::request_type_tostr(RequestType request_type, LoggingBuffer* logging_buffer){
    switch (request_type) {
        case Static:
            return "Static";
        case Echo:
            return "Echo";
        case CRUD:
            return "CRUD";
        case Health:
            return "Health";
        case Sleep:
            return "Sleep";
        case None:
            return "Not Found";
        case BogusType:
            return "BogusType";
        default:
            return "Unknown";
    }
}


http::response<http::vector_body<char>> RequestDispatcher::dispatch_request(const http::request<http::vector_body<char>>& request, const boost::asio::ip::tcp::socket& socket, LoggingBuffer* logging_buffer){

        //Determine the request type and get request object
        RequestType request_type = RequestDispatcher::getRequestType(request, logging_buffer);
        boost::system::error_code ec;

        logging_buffer->addToBuffer(INFO, "Request from " + socket.remote_endpoint(ec).address().to_string() + " for target " + request.target().to_string());
        if (ec){
            logging_buffer->addToBuffer(ERROR, "handle_read: Transport endpoint is not connected");
        }

        http::response<http::vector_body<char>> response;
        //Check for empty request or unknown method
        if (request.target().empty() || request.method() == http::verb::unknown){
            logging_buffer->addToBuffer(ERROR, "Empty request or unknown method");
            std::vector<char> body;
            response = http::response<http::vector_body<char>>(http::status::bad_request, 11U, body);
            response.prepare_payload();
        }
        //Otherwise, dispatch request to appropriate handler
        else{
            RequestHandler* rh =  Registry::GetRequestHandler(request_type, logging_buffer);
            response = rh->handle_request(request);
        }

        std::stringstream s;

        s << "[ResponseMetrics]" << "\t" 
                                << "Request IP: " << socket.remote_endpoint(ec).address()  << "\t"
                                << "Request Target: " << request.target() << "\t"
                                << "Response Code: " << response.result_int() << "\t"
                                << "Response Message: " << response.result() << "\t"
                                << "Response Handler: " << RequestDispatcher::request_type_tostr(request_type, logging_buffer);

        logging_buffer->addToBuffer(INFO, s.str());

        return response;
}
