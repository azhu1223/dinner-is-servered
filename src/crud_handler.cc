#include "request_handler.h"
#include "crud_handler.h"
#include "utils.h"
#include <boost/log/trivial.hpp>
#include <fstream>
#include <vector>
#include <string>
#include "request_dispatcher.h"

CrudHandler::CrudHandler() : RequestHandler() {}

http::response<http::vector_body<char>> CrudHandler::handle_request(const http::request<http::vector_body<char>>& req) {
    http::response<http::vector_body<char>> response;

    BOOST_LOG_TRIVIAL(info) << "CRUD request method: " << req.method_string();
    std::string req_method{req.method_string()};

    // TODO: parse the Entity + Entity id from the url
    if (req_method == "POST") {
      // TODO: generate id with uuid, match against existing ids 
      // parse JSON in request, write to filesystem, and return JSON with id
      BOOST_LOG_TRIVIAL(error) << "Unimplemented: POST";
    } else if (req_method == "GET") {
      // TODO: lookup the id in the filesystem, and return JSON 
      BOOST_LOG_TRIVIAL(error) << "Unimplemented: GET";
    } else if (req_method == "PUT") {
      // TODO
      BOOST_LOG_TRIVIAL(error) << "Unimplemented: PUT";
    } else if (req_method == "DELETE") {
      // TODO
      BOOST_LOG_TRIVIAL(error) << "Unimplemented: DELETE";
    } else {
      BOOST_LOG_TRIVIAL(error) << "Invalid CRUD request method. Must be one of POST/GET/PUT/DELETE";
      std::string response_body_string = "400 Bad Request \r\n\r\n";
      std::vector<char> response_body_vector(response_body_string.begin(), response_body_string.end());
      response = http::response<http::vector_body<char>>(http::status::bad_request, 11U);
      response.set(http::field::content_type, "text/plain");
      response.set(http::field::content_length, std::to_string(response_body_vector.size()));
      response.prepare_payload();
      return response;
    }

    std::string response_body_string = "501 Not Implemented \r\n\r\n";
    std::vector<char> response_body_vector(response_body_string.begin(), response_body_string.end());
    response = http::response<http::vector_body<char>>(http::status::not_implemented, 11U);
    response.set(http::field::content_type, "text/plain");
    response.set(http::field::content_length, std::to_string(response_body_vector.size()));
    response.prepare_payload();
    return response;
}

RequestHandler* CrudHandlerFactory::create() {
    return new CrudHandler();
}