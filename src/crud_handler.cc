#include "request_handler.h"
#include "crud_handler.h"
#include "utils.h"
#include <boost/log/trivial.hpp>
#include <fstream>
#include <vector>
#include <string>
#include "request_dispatcher.h"

#include "crud_file_manager.h"
#include <sstream>

CrudHandler::CrudHandler(CrudFileManager manager) : RequestHandler(), file_manager(manager) {}

std::string CrudHandler::generateEntityID(std::unordered_set<std::string> used_ids) {
  // TODO: add implementation, locking for thread safety
  return "1";
}

http::response<http::vector_body<char>> CrudHandler::handle_request(const http::request<http::vector_body<char>>& req) {
    http::response<http::vector_body<char>> response;
    CrudPath path = RequestDispatcher::getCrudEntityPath(req);

    if (req.method() == http::verb::post) {
      if (path.entity_id != "") {
        BOOST_LOG_TRIVIAL(error) << "Invalid CRUD POST request. Request may not contain Entity ID";
        std::string response_body_string = "400 Bad Request \r\n\r\n";
        std::vector<char> response_body_vector(response_body_string.begin(), response_body_string.end());
        response = http::response<http::vector_body<char>>(http::status::bad_request, 11U, response_body_vector);
        response.set(http::field::content_type, "text/plain");
        response.set(http::field::content_length, std::to_string(response_body_vector.size()));
        response.prepare_payload();
        return response;
      }

      // (1) generate unique id for the Entity
      std::unordered_set<std::string> used_ids;
      std::string new_id = CrudHandler::generateEntityID(used_ids);
      path.entity_id = new_id;

      // (2) get the data in POST body
      std::vector<char> req_body(req.body());
      std::string req_body_string(req_body.begin(),req_body.end());
      BOOST_LOG_TRIVIAL(info) << "POST request body: " << req_body_string;

      // (3) write the Entity data to filesystem
      file_manager.writeObject(path, req_body_string);

      // (4) return the ID of newly created Entity
      std::string response_body_string = "{ \"id\": \"" + new_id + "\" }";
      std::vector<char> response_body_vector(response_body_string.begin(), response_body_string.end());
      response = http::response<http::vector_body<char>>(http::status::created, 11U, response_body_vector);
      response.set(http::field::content_type, "text/plain");
      response.set(http::field::content_length, std::to_string(response_body_vector.size()));
      response.prepare_payload();
      return response;

    } else if (req.method() == http::verb::get) {
      // TODO: lookup the id in the filesystem, and return JSON 
      BOOST_LOG_TRIVIAL(error) << "Unimplemented: GET";
    } else if (req.method() == http::verb::put) {
      // TODO
      BOOST_LOG_TRIVIAL(error) << "Unimplemented: PUT";
    } else if (req.method() == http::verb::delete_) {
      // TODO
      BOOST_LOG_TRIVIAL(error) << "Unimplemented: DELETE";
    } else {
      BOOST_LOG_TRIVIAL(error) << "Invalid CRUD request method. Must be one of POST/GET/PUT/DELETE";
      std::string response_body_string = "400 Bad Request \r\n\r\n";
      std::vector<char> response_body_vector(response_body_string.begin(), response_body_string.end());
      response = http::response<http::vector_body<char>>(http::status::bad_request, 11U, response_body_vector);
      response.set(http::field::content_type, "text/plain");
      response.set(http::field::content_length, std::to_string(response_body_vector.size()));
      response.prepare_payload();
      return response;
    }

    std::string response_body_string = "501 Not Implemented \r\n\r\n";
    std::vector<char> response_body_vector(response_body_string.begin(), response_body_string.end());
    response = http::response<http::vector_body<char>>(http::status::not_implemented, 11U, response_body_vector);
    response.set(http::field::content_type, "text/plain");
    response.set(http::field::content_length, std::to_string(response_body_vector.size()));
    response.prepare_payload();
    return response;
}

RequestHandler* CrudHandlerFactory::create() {
    return new CrudHandler(CrudFileManager());
}