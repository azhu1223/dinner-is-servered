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


RequestType RequestDispatcher::getRequestType(const boost::beast::http::request<boost::beast::http::vector_body<char>>& request){
    boost::beast::string_view target = request.target();
    BOOST_LOG_TRIVIAL(info) << "Paths are: ";
    ServerPaths server_paths = ConfigInterpreter::getServerPaths();
    BOOST_LOG_TRIVIAL(info) << "Determining request type. Target is: " << target;
    BOOST_LOG_TRIVIAL(info) << "Paths are: ";
    for (auto path : server_paths.static_){
        BOOST_LOG_TRIVIAL(info) << path.first;
    }
    for (auto path : server_paths.echo_){
        BOOST_LOG_TRIVIAL(info) << path;
    }
    for (auto path : server_paths.crud_){
        BOOST_LOG_TRIVIAL(info) << path.first;
    }



    std::string result;
    bool is_prev_shash = false;
    RequestType request_type = None;

    //Remove back to back slashes and trailing slashes
    for (char c : target) {
        if (c == '/' && is_prev_shash) {
            BOOST_LOG_TRIVIAL(info) << "Found multiple slashes in a row";
            continue;
        }
        result += c;
        is_prev_shash = (c == '/');
    }
    while (result.back() == '/' && result.length() > 1) {
        BOOST_LOG_TRIVIAL(info) << "Found trailing slash";
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
        BOOST_LOG_TRIVIAL(error) << "There was no matching path found for the request. 404 Handler should be invoked";
        return request_type;
    }
    else if (request_type == Static){
        std::ifstream file(server_paths.static_[longest_path] + result.substr(result.find(longest_path) + longest_path.length()),
            std::ios::in | std::ios::binary);
        if (!file.is_open()) {
            BOOST_LOG_TRIVIAL(error) << "File not found. 404 Handler should be invoked";
            request_type = None;
            return request_type;
        }
    }


    BOOST_LOG_TRIVIAL(info) << "Request is of type " << RequestDispatcher::request_type_tostr(request_type);
    return request_type;
}

std::string RequestDispatcher::getStaticFilePath
    (const boost::beast::http::request<boost::beast::http::vector_body<char>>& request){

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
        BOOST_LOG_TRIVIAL(error) << "getStaticFilePath was called for a non-static request";
        return "";
    }


    std::string file_path = server_paths.static_[longest_path] 
                        + result.substr(result.find(longest_path) + longest_path.length());
    BOOST_LOG_TRIVIAL(info) << "Set file_path_ to " << file_path;
    return file_path;
};

CrudPath RequestDispatcher::getCrudEntityPath
    (const boost::beast::http::request<boost::beast::http::vector_body<char>>& request){

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
        BOOST_LOG_TRIVIAL(error) << "getCrudEntityID was called for a non-CRUD request";
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
    BOOST_LOG_TRIVIAL(info) << "entity: " << entity_name;
    BOOST_LOG_TRIVIAL(info) << "id: " << entity_id;

    CrudPath crud_path;
    crud_path.data_path = server_paths.crud_[longest_path];
    crud_path.entity_name = entity_name;
    crud_path.entity_id = entity_id;

    return crud_path;
};

std::string RequestDispatcher::request_type_tostr(RequestType request_type){
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


http::response<http::vector_body<char>> RequestDispatcher::dispatch_request(const boost::beast::http::request<boost::beast::http::vector_body<char>>& request, const boost::asio::ip::tcp::socket& socket){

        //Determine the request type and get request object
        RequestType request_type = RequestDispatcher::getRequestType(request);
        boost::system::error_code ec;

        BOOST_LOG_TRIVIAL(info) << "Request from " << socket.remote_endpoint(ec).address() 
            << " for target " << request.target();
        if (ec){
            BOOST_LOG_TRIVIAL(error) << "handle_read: Transport endpoint is not connected";
        }

        http::response<http::vector_body<char>> response;
        //Check for empty request or unknown method
        if (request.target().empty() || request.method() == boost::beast::http::verb::unknown){
            BOOST_LOG_TRIVIAL(error) << "Empty request or unknown method";
            std::vector<char> body;
            response = http::response<http::vector_body<char>>(http::status::bad_request, 11U, body);
            response.prepare_payload();
        }
        //Otherwise, dispatch request to appropriate handler
        else{
            RequestHandler* rh =  Registry::GetRequestHandler(request_type);
            response = rh->handle_request(request);
        }

        BOOST_LOG_TRIVIAL(info) << "[ResponseMetrics]" << "\t" 
                                << "Request IP: " << socket.remote_endpoint(ec).address()  << "\t"
                                << "Request Target: " << request.target() << "\t"
                                << "Response Code: " << response.result_int() << "\t"
                                << "Response Message: " << response.result() << "\t"
                                << "Response Handler: " << RequestDispatcher::request_type_tostr(request_type);

        return response;
}
