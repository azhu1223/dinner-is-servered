#ifndef REQUEST_PARSER_H
#define REQUEST_PARSER_H

#include "utils.h"
#include <boost/beast/http/parser.hpp>
#include <boost/beast/http.hpp>
#include <boost/asio.hpp>
#include <string>
#include "crud_file_manager.h"
#include "logging_buffer.h"

namespace http = boost::beast::http;

enum RequestType {
    //When adding types to the enum, be sure to modify the request_type_tostr function in the .cc file
    Static,
    Echo,
    None,
    CRUD,
    Health,
    Sleep,
    App,
    BogusType // Just for testing purposes, no functional relevance.
};

class RequestDispatcher {

    public:
        static RequestType getRequestType(const http::request<http::vector_body<char>>& request, LoggingBuffer* logging_buffer);
        static std::string getStaticFilePath(const http::request<http::vector_body<char>>& request, LoggingBuffer* logging_buffer);
        static CrudPath getCrudEntityPath(const http::request<http::vector_body<char>>& request, LoggingBuffer* logging_buffer);
        static std::string request_type_tostr(RequestType request_type, LoggingBuffer* logging_buffer);
        static http::response<http::vector_body<char>> dispatch_request(const http::request<http::vector_body<char>>& request, const boost::asio::ip::tcp::socket& socket, LoggingBuffer* logging_buffer);
};

#endif