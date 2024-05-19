#include "request_dispatcher.h"
#include "config_interpreter.h"
#include "utils.h"
#include <boost/beast/http/parser.hpp>
#include <boost/beast/http.hpp>
#include <boost/log/trivial.hpp>
#include <fstream>

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


    BOOST_LOG_TRIVIAL(info) << "Request is of type " << request_type;
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