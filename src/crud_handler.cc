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

CrudHandler::CrudHandler(std::shared_ptr<CrudFileManager> manager) : RequestHandler(), file_manager(manager) {}

CrudHandler::~CrudHandler() {
}

std::string CrudHandler::generateEntityID(std::unordered_set<std::string> used_ids) {
  // TODO: add implementation, locking for thread safety
  return "1";
}

http::response<http::vector_body<char>> CrudHandler::handle_post(CrudPath path, const http::request<http::vector_body<char>>& req) {
  http::response<http::vector_body<char>> response;
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
  file_manager->writeObject(path, req_body_string);

  // (4) return the ID of newly created Entity
  std::string response_body_string = "{ \"id\": \"" + new_id + "\" }";
  std::vector<char> response_body_vector(response_body_string.begin(), response_body_string.end());
  response = http::response<http::vector_body<char>>(http::status::created, 11U, response_body_vector);
  response.set(http::field::content_type, "text/plain");
  response.set(http::field::content_length, std::to_string(response_body_vector.size()));
  response.prepare_payload();
  return response;
}

http::response<http::vector_body<char>> CrudHandler::handle_get(CrudPath path, const http::request<http::vector_body<char>>& req) {
  http::response<http::vector_body<char>> response;
  if (path.entity_id == "") {
    // TODO: implement List Request
    std::string response_body_string = "501 Not Implemented \r\n\r\n";
    std::vector<char> response_body_vector(response_body_string.begin(), response_body_string.end());
    response = http::response<http::vector_body<char>>(http::status::not_implemented, 11U, response_body_vector);
    response.set(http::field::content_type, "text/plain");
    response.set(http::field::content_length, std::to_string(response_body_vector.size()));
    response.prepare_payload();
    return response;
  }

  // (1) read file with (Entity, id) from filesystem
  std::string file_contents;
  bool read_success = file_manager->readObject(path, file_contents);
  if (!read_success) {
    BOOST_LOG_TRIVIAL(error) << "Error reading requested file.";
    std::string response_body_string = "404 Not Found\r\n\r\n";
    std::vector<char> response_body_vector(response_body_string.begin(), response_body_string.end());
    response = http::response<http::vector_body<char>>(http::status::not_found, 11U, response_body_vector);
    response.set(http::field::content_type, "text/plain");
    response.set(http::field::content_length, std::to_string(response_body_vector.size()));
    response.prepare_payload();
    return response;
  }

  // (2) return the requested file contents to user
  std::vector<char> response_body_vector(file_contents.begin(), file_contents.end());
  response = http::response<http::vector_body<char>>(http::status::ok, 11U, response_body_vector);
  response.set(http::field::content_type, "application/json");
  response.set(http::field::content_length, std::to_string(response_body_vector.size()));
  response.prepare_payload();
  return response; 
}

http::response<http::vector_body<char>> CrudHandler::handle_request(const http::request<http::vector_body<char>>& req) {
    http::response<http::vector_body<char>> response;
    CrudPath path = RequestDispatcher::getCrudEntityPath(req);

    bool empty_entity_name = path.entity_name == ""; 
    bool empty_entity_id = path.entity_id == ""; 

    if (req.method() == http::verb::post) {
      return handle_post(path, req);
    } else if (req.method() == http::verb::get) {
      return handle_get(path, req);
    } else if (req.method() == http::verb::put) {

      // (1) First, ensure that the entity_name and entity_id is present 
      if (empty_entity_name || empty_entity_id) {
        BOOST_LOG_TRIVIAL(error) << "Invalid CRUD POST request. Request must contain Entity Name and Entity ID";
        std::string response_body_string = "400 Bad Request \r\n\r\n";
        std::vector<char> response_body_vector(response_body_string.begin(), response_body_string.end());
        response = http::response<http::vector_body<char>>(http::status::bad_request, 11U, response_body_vector);
        response.set(http::field::content_type, "text/plain");
        response.set(http::field::content_length, std::to_string(response_body_vector.size()));
        response.prepare_payload();
        return response;
      }

      // (2) Next, ensure that this entity id corresponds to an existing object (is valid)
      bool entity_exists = file_manager->existsObject(path); 

      if (!entity_exists) {
        BOOST_LOG_TRIVIAL(error) << "404 Error. Entity object does not exist.";
        std::string response_body_string = "404 Not Found \r\n\r\n";
        std::vector<char> response_body_vector(response_body_string.begin(), response_body_string.end());
        response = http::response<http::vector_body<char>>(http::status::not_found, 11U, response_body_vector);
        response.set(http::field::content_type, "text/plain");
        response.set(http::field::content_length, std::to_string(response_body_vector.size()));
        response.prepare_payload();
        return response;
      }

      // (3) If the object at /data_path/entity_name/entity_id exists, update its data 

      // (3a) Get the data in PUT body
      std::vector<char> req_body(req.body());
      std::string req_body_string(req_body.begin(),req_body.end());
      BOOST_LOG_TRIVIAL(info) << "PUT request body: " << req_body_string;

      // (3b) Write the Entity data to filesystem
      file_manager->writeObject(path, req_body_string); // Overloading this since file confirmed to exist

      // (3c) Return success message 
      std::string response_body_string = "Successful update.";
      std::vector<char> response_body_vector(response_body_string.begin(), response_body_string.end());
      response = http::response<http::vector_body<char>>(http::status::ok, 11U, response_body_vector);
      response.set(http::field::content_type, "text/plain");
      response.set(http::field::content_length, std::to_string(response_body_vector.size()));
      response.prepare_payload();
      return response;

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
    return new CrudHandler(std::make_shared<CrudFileManager>());
}