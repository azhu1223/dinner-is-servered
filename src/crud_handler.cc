#include "request_handler.h"
#include "crud_handler.h"
#include "utils.h"
#include <fstream>
#include <vector>
#include <string>
#include "request_dispatcher.h"
#include "logging_buffer.h"

#include "crud_file_manager.h"
#include <sstream>

CrudHandler::CrudHandler(std::shared_ptr<CrudFileManager> manager, LoggingBuffer* logging_buffer) : RequestHandler(), file_manager(manager), logging_buffer_(logging_buffer){}

CrudHandler::~CrudHandler() {
}

http::response<http::vector_body<char>> CrudHandler::handle_post(CrudPath path, const http::request<http::vector_body<char>>& req) {
  http::response<http::vector_body<char>> response;
  if (path.entity_id != "") {
    logging_buffer_->addToBuffer(ERROR, "Invalid CRUD POST request. Request may not contain Entity ID");
    std::string response_body_string = "400 Bad Request \r\n\r\n";
    std::vector<char> response_body_vector(response_body_string.begin(), response_body_string.end());
    response = http::response<http::vector_body<char>>(http::status::bad_request, 11U, response_body_vector);
    response.set(http::field::content_type, "text/plain");
    response.set(http::field::content_length, std::to_string(response_body_vector.size()));
    response.prepare_payload();
    return response;
  }

  // (1) generate unique id for the Entity
  std::string new_id = file_manager->generateEntityId(path);
  path.entity_id = std::to_string(std::stoull(new_id));

  // (2) get the data in POST body
  std::vector<char> req_body(req.body());
  std::string req_body_string(req_body.begin(),req_body.end());
  logging_buffer_->addToBuffer(INFO, "POST request body: " + req_body_string);

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

http::response<http::vector_body<char>> CrudHandler::handle_list(CrudPath path, const http::request<http::vector_body<char>>& req) {
  http::response<http::vector_body<char>> response;
  // (1) Check if entity_name is present 
  if (path.entity_name == "") {
    logging_buffer_->addToBuffer(ERROR, "Invalid CRUD GET request. Request must contain Entity Name");
    std::string response_body_string = "400 Bad Request \r\n\r\n";
    std::vector<char> response_body_vector(response_body_string.begin(), response_body_string.end());
    response = http::response<http::vector_body<char>>(http::status::bad_request, 11U, response_body_vector);
    response.set(http::field::content_type, "text/plain");
    response.set(http::field::content_length, std::to_string(response_body_vector.size()));
    response.prepare_payload();
    return response;
  }

  // (2) Look at files in path.data_path + "/" + path.entity_name and 
  std::vector<std::string> file_names_list = file_manager->listObjects(path); 

  // (3) Turn vector into string of list of file names (ex. [1, 2, 3])
  std::ostringstream oss;
  oss << "[";
  for (size_t i = 0; i < file_names_list.size(); ++i) {
      oss << file_names_list[i];
      if (i < file_names_list.size() - 1) {
          oss << ", ";
      }
  }
  oss << "]";
  std::string file_contents = oss.str();

  // (4) Send ok response with file_contents payload 
  std::vector<char> response_body_vector(file_contents.begin(), file_contents.end());
  response = http::response<http::vector_body<char>>(http::status::ok, 11U, response_body_vector);
  response.set(http::field::content_type, "application/json");
  response.set(http::field::content_length, std::to_string(response_body_vector.size()));
  response.prepare_payload();
  return response; 
}

http::response<http::vector_body<char>> CrudHandler::handle_get(CrudPath path, const http::request<http::vector_body<char>>& req) {
  http::response<http::vector_body<char>> response;
  if (path.entity_id == "") {
    return handle_list(path, req); 
  }

  // (1) read file with (Entity, id) from filesystem
  std::string file_contents;
  bool read_success = file_manager->readObject(path, file_contents);
  if (!read_success) {
    logging_buffer_->addToBuffer(ERROR, "Error reading requested file.");
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

http::response<http::vector_body<char>> CrudHandler::handle_put(CrudPath path, const http::request<http::vector_body<char>>& req) {
  http::response<http::vector_body<char>> response;

  bool empty_entity_name = path.entity_name == ""; 
  bool empty_entity_id = path.entity_id == ""; 

  // (1) First, ensure that the entity_name and entity_id is present 
  if (empty_entity_name || empty_entity_id) {
    logging_buffer_->addToBuffer(ERROR, "Invalid CRUD POST request. Request must contain Entity Name and Entity ID");
    std::string response_body_string = "400 Bad Request \r\n\r\n";
    std::vector<char> response_body_vector(response_body_string.begin(), response_body_string.end());
    response = http::response<http::vector_body<char>>(http::status::bad_request, 11U, response_body_vector);
    response.set(http::field::content_type, "text/plain");
    response.set(http::field::content_length, std::to_string(response_body_vector.size()));
    response.prepare_payload();
    return response;
  }

  // (2) Valid PUT request, so get the data in the PUT body
  std::vector<char> req_body(req.body());
  std::string req_body_string(req_body.begin(),req_body.end());
  logging_buffer_->addToBuffer(INFO, "PUT request body: " + req_body_string);

  // (3) Next, check if this entity id corresponds to an existing object 
  bool entity_exists = file_manager->existsObject(path); 

  // (4) If object exists, will overwrite | If not, will create new object 
  file_manager->writeObject(path, req_body_string);

  // (4a) If entity_id did not exist, return ID of newly created entity
  // (4b) Else, return success response (indicating successful update)
  if (!entity_exists) {
      std::string response_body_string = "{ \"id\": \"" + path.entity_id + "\" }";
      std::vector<char> response_body_vector(response_body_string.begin(), response_body_string.end());
      response = http::response<http::vector_body<char>>(http::status::created, 11U, response_body_vector);
      response.set(http::field::content_type, "text/plain");
      response.set(http::field::content_length, std::to_string(response_body_vector.size()));
      response.prepare_payload();
      return response;
  } else {
    std::string response_body_string = "Successful update.";
    std::vector<char> response_body_vector(response_body_string.begin(), response_body_string.end());
    response = http::response<http::vector_body<char>>(http::status::ok, 11U, response_body_vector);
    response.set(http::field::content_type, "text/plain");
    response.set(http::field::content_length, std::to_string(response_body_vector.size()));
    response.prepare_payload();
    return response;
  }
}

http::response<http::vector_body<char>> CrudHandler::handle_del(CrudPath path, const http::request<http::vector_body<char>>& req) {
  http::response<http::vector_body<char>> response;
  
  bool empty_entity_name = path.entity_name == ""; 
  bool empty_entity_id = path.entity_id == ""; 

  // (1) First, ensure that the entity_name and entity_id is present 
  if (empty_entity_name || empty_entity_id) {
    logging_buffer_->addToBuffer(ERROR, "Invalid CRUD DELETE request. Request must contain Entity Name and Entity ID");
    std::string response_body_string = "400 Bad Request \r\n\r\n";
    std::vector<char> response_body_vector(response_body_string.begin(), response_body_string.end());
    response = http::response<http::vector_body<char>>(http::status::bad_request, 11U, response_body_vector);
    response.set(http::field::content_type, "text/plain");
    response.set(http::field::content_length, std::to_string(response_body_vector.size()));
    response.prepare_payload();
    return response;
  }

  // (2) Next, delete the file 
  bool object_existed = file_manager->deleteObject(path);

  if (object_existed) { 
    std::string response_body_string = "Successful delete.";
    std::vector<char> response_body_vector(response_body_string.begin(), response_body_string.end());
    response = http::response<http::vector_body<char>>(http::status::ok, 11U, response_body_vector);
    response.set(http::field::content_type, "text/plain");
    response.set(http::field::content_length, std::to_string(response_body_vector.size()));
    response.prepare_payload();
    return response;
  } else {
    std::string response_body_string = ""; // 204 = no_content
    std::vector<char> response_body_vector(response_body_string.begin(), response_body_string.end());
    response = http::response<http::vector_body<char>>(http::status::no_content, 11U, response_body_vector);
    response.set(http::field::content_type, "text/plain");
    response.set(http::field::content_length, std::to_string(response_body_vector.size()));
    response.prepare_payload();
    return response;
  }
}

http::response<http::vector_body<char>> CrudHandler::handle_request(const http::request<http::vector_body<char>>& req) {
    http::response<http::vector_body<char>> response;
    CrudPath path = RequestDispatcher::getCrudEntityPath(req, logging_buffer_);


    //Ensure that the method is GET, POST, PUT, or DELETE
    if (req.method() != http::verb::get && req.method() != http::verb::post && req.method() != http::verb::put && req.method() != http::verb::delete_) {
      logging_buffer_->addToBuffer(ERROR, "Invalid CRUD request method. Must be one of POST/GET/PUT/DELETE");
      std::string response_body_string = "400 Bad Request\r\nRequest method must be one of POST/GET/PUT/DELETE \r\n\r\n";
      std::vector<char> response_body_vector(response_body_string.begin(), response_body_string.end());
      response = http::response<http::vector_body<char>>(http::status::bad_request, 11U, response_body_vector);
      response.set(http::field::content_type, "text/plain");
      response.set(http::field::content_length, std::to_string(response_body_vector.size()));
      response.prepare_payload();
      return response;
    }
    //Otherwise, ensure that an entity is present
    else if (path.entity_name == "") {
      logging_buffer_->addToBuffer(ERROR, "Invalid CRUD request. Missing Entity name");
      std::string response_body_string = "400 Bad Request\r\nRequest must contain Entity name \r\n\r\n";
      std::vector<char> response_body_vector(response_body_string.begin(), response_body_string.end());
      response = http::response<http::vector_body<char>>(http::status::bad_request, 11U, response_body_vector);
      response.set(http::field::content_type, "text/plain");
      response.set(http::field::content_length, std::to_string(response_body_vector.size()));
      response.prepare_payload();
      return response;
    }

    //Dispatch the appropriate method to the appropriate handler
    if (req.method() == http::verb::post) {
      return handle_post(path, req);
    } else if (req.method() == http::verb::get) {
      return handle_get(path, req);
    } else if (req.method() == http::verb::put) {
      return handle_put(path, req);
    } else if (req.method() == http::verb::delete_) {
      return handle_del(path, req);
    }


      logging_buffer_->addToBuffer(ERROR, "Should never reach here. All cases should be handled above");
      std::vector<char> response_body_vector;;
      response = http::response<http::vector_body<char>>(http::status::bad_request, 11U, response_body_vector);
      response.prepare_payload();
      return response;
}

RequestHandler* CrudHandlerFactory::create(LoggingBuffer* logging_buffer) {
    return new CrudHandler(std::make_shared<CrudFileManager>(logging_buffer), logging_buffer);
}