#include <gtest/gtest.h>
#include "crud_handler.h"
#include <string>
#include <vector>
#include <fstream>
#include <filesystem>
#include "config_interpreter.h"
#include <gmock/gmock.h>
#include <boost/beast/http.hpp>
#include "request_dispatcher.h"
#include "crud_file_manager.h"
#include <unordered_map>
namespace http = boost::beast::http;

class FakeCrudFileManager : public CrudFileManager {
  public:
    virtual bool readObject(CrudPath path, std::string& json) override;
    virtual bool writeObject(CrudPath path, std::string json) override;
  private:
    std::unordered_map<std::string,std::string> filesystem;
};

bool FakeCrudFileManager::readObject(CrudPath path, std::string& json) {
  std::string file_path = path.entity_name + "/" + path.entity_id;
  if (filesystem.find(file_path) == filesystem.end()) return false;
  json = filesystem[file_path];
  return true;
}

bool FakeCrudFileManager::writeObject(CrudPath path, std::string json) {
  std::string file_path = path.entity_name + "/" + path.entity_id;
  filesystem[file_path] = json;
  return true;
}

class CrudHandlerTest : public ::testing::Test {
protected:
    void SetUp() override {
        handler = new CrudHandler(std::make_shared<FakeCrudFileManager>());
    }

    void TearDown() override {
        delete handler;
    }

    CrudHandler* handler;
};

// (1) POST Tests 

TEST_F(CrudHandlerTest, PostRequestSuccessTest) {
    std::vector<std::string> echo_paths;
    std::map<std::string, std::string> static_paths;
    std::map<std::string, std::string> crud_paths;
    echo_paths.push_back("/echo1");
    static_paths["/static"] = "./static";
    crud_paths["/api"] = "./data";
    ServerPaths sp;
    sp.echo_ = echo_paths;
    sp.static_ = static_paths;
    sp.crud_ = crud_paths;
    ConfigInterpreter::setServerPaths(sp);

    std::string string_body = "{ \"data\": \"1\"}";
    std::vector<char> post_body(string_body.begin(),string_body.end());
    http::request<http::vector_body<char>> req;
    req.method(http::verb::post);
    req.target("/api/PostRequestTest");
    req.version(11);
    req.body() = post_body;
    req.prepare_payload();

    std::shared_ptr<FakeCrudFileManager> manager = std::make_shared<FakeCrudFileManager>();
    CrudHandler handler(manager);
    http::response<http::vector_body<char>> response = handler.handle_request(req);

    EXPECT_EQ(response.result(), http::status::created);

    int deleted = std::filesystem::remove_all(crud_paths["/api"] );
}

TEST_F(CrudHandlerTest, PostRequestFailTest) {
    std::map<std::string, std::string> crud_paths;
    crud_paths["/api"] = "./data";
    ServerPaths sp;
    sp.crud_ = crud_paths;
    ConfigInterpreter::setServerPaths(sp);

    std::string string_body = "{ \"data\": \"1\"}";
    std::vector<char> post_body(string_body.begin(),string_body.end());
    http::request<http::vector_body<char>> req;
    req.method(http::verb::post);
    req.target("/api/PostRequestTest/1");
    req.version(11);
    req.body() = post_body;
    req.prepare_payload();

    std::shared_ptr<FakeCrudFileManager> manager = std::make_shared<FakeCrudFileManager>();
    CrudHandler handler(manager);
    http::response<http::vector_body<char>> response = handler.handle_request(req);

    EXPECT_EQ(response.result(), http::status::bad_request);
}

// (2) PUT Tests 

TEST_F(CrudHandlerTest, PutRequestSuccessTest) {
    std::vector<std::string> echo_paths;
    std::map<std::string, std::string> static_paths;
    std::map<std::string, std::string> crud_paths;
    echo_paths.push_back("/echo1");
    static_paths["/static"] = "./static";
    crud_paths["/api"] = "./data";
    ServerPaths sp;
    sp.echo_ = echo_paths;
    sp.static_ = static_paths;
    sp.crud_ = crud_paths;
    ConfigInterpreter::setServerPaths(sp);

    // First, need to create an object so we can update it 
    std::string string_body = "{ \"data\": \"1\"}";
    std::vector<char> post_body(string_body.begin(),string_body.end());
    http::request<http::vector_body<char>> req;
    req.method(http::verb::post);
    req.target("/api/Books");
    req.version(11);
    req.body() = post_body;
    req.prepare_payload();

     std::shared_ptr<CrudFileManager> manager = std::make_shared<CrudFileManager>();;
    CrudHandler handler(manager);
    http::response<http::vector_body<char>> response = handler.handle_request(req);
    EXPECT_EQ(response.result(), http::status::created);

    // Now, let's update this object 
    string_body = "{ \"data\": \"2\"}";
    std::vector<char> put_body(string_body.begin(),string_body.end());
    req.method(http::verb::put);
    req.target("/api/Books/1");
    req.version(11);
    req.body() = put_body;
    req.prepare_payload();

    response = handler.handle_request(req);
    EXPECT_EQ(response.result(), http::status::ok);

    // Now, check if file content updated properly 
    std::ifstream file("./data/Books/1");
    EXPECT_TRUE(file.is_open());

    std::stringstream buffer;
    buffer << file.rdbuf();
    EXPECT_EQ(buffer.str(), string_body); 

    int deleted = std::filesystem::remove_all(crud_paths["/api"] );
}

TEST_F(CrudHandlerTest, PutRequestFailTest_NoObject) {
    std::vector<std::string> echo_paths;
    std::map<std::string, std::string> static_paths;
    std::map<std::string, std::string> crud_paths;
    echo_paths.push_back("/echo1");
    static_paths["/static"] = "./static";
    crud_paths["/api"] = "./data";
    ServerPaths sp;
    sp.echo_ = echo_paths;
    sp.static_ = static_paths;
    sp.crud_ = crud_paths;
    ConfigInterpreter::setServerPaths(sp);

    // Try to update an object that doesn't exist --> will create it!
    std::string string_body = "{ \"data\": \"1\"}";
    std::vector<char> put_body(string_body.begin(),string_body.end());
    http::request<http::vector_body<char>> req;
    req.method(http::verb::put);
    req.target("/api/Books/1");
    req.version(11);
    req.body() = put_body;
    req.prepare_payload();

    std::shared_ptr<CrudFileManager> manager = std::make_shared<CrudFileManager>();
    CrudHandler handler(manager);
    http::response<http::vector_body<char>> response = handler.handle_request(req);
    EXPECT_EQ(response.result(), http::status::created);
}

TEST_F(CrudHandlerTest, PutRequestFailTest_EmptyId) {
    std::vector<std::string> echo_paths;
    std::map<std::string, std::string> static_paths;
    std::map<std::string, std::string> crud_paths;
    echo_paths.push_back("/echo1");
    static_paths["/static"] = "./static";
    crud_paths["/api"] = "./data";
    ServerPaths sp;
    sp.echo_ = echo_paths;
    sp.static_ = static_paths;
    sp.crud_ = crud_paths;
    ConfigInterpreter::setServerPaths(sp);

    // Trying with empty entity_id (/api/Books)
    std::string string_body = "{ \"data\": \"1\"}";
    std::vector<char> put_body(string_body.begin(),string_body.end());
    http::request<http::vector_body<char>> req;
    req.method(http::verb::put);
    req.target("/api/Books");
    req.version(11);
    req.body() = put_body;
    req.prepare_payload();

    std::shared_ptr<CrudFileManager> manager = std::make_shared<CrudFileManager>();
    CrudHandler handler(manager);
    http::response<http::vector_body<char>> response = handler.handle_request(req);
    EXPECT_EQ(response.result(), http::status::bad_request);
}

// (3) GET Tests

TEST_F(CrudHandlerTest, GetRequestSuccessTest) {
    std::map<std::string, std::string> crud_paths;
    crud_paths["/api"] = "./data";
    ServerPaths sp;
    sp.crud_ = crud_paths;
    ConfigInterpreter::setServerPaths(sp);

    std::string string_body = "{ \"data\": \"1\"}";
    std::vector<char> post_body(string_body.begin(),string_body.end());
    http::request<http::vector_body<char>> post_request;
    post_request.method(http::verb::post);
    post_request.target("/api/GetRequestTest");
    post_request.version(11);
    post_request.body() = post_body;
    post_request.prepare_payload();

    std::shared_ptr<FakeCrudFileManager> manager = std::make_shared<FakeCrudFileManager>();
    CrudHandler handler(manager);
    handler.handle_request(post_request);

    http::request<http::vector_body<char>> get_request;
    get_request.method(http::verb::get);
    get_request.target("/api/GetRequestTest/1");
    get_request.version(11);
    get_request.prepare_payload();

    http::response<http::vector_body<char>> response = handler.handle_request(get_request);

    EXPECT_EQ(response.result(), http::status::ok);
}

TEST_F(CrudHandlerTest, GetRequestFailNotFoundTest) {
    std::map<std::string, std::string> crud_paths;
    crud_paths["/api"] = "./data";
    ServerPaths sp;
    sp.crud_ = crud_paths;
    ConfigInterpreter::setServerPaths(sp);

    std::shared_ptr<FakeCrudFileManager> manager = std::make_shared<FakeCrudFileManager>();
    CrudHandler handler(manager);

    http::request<http::vector_body<char>> get_request;
    get_request.method(http::verb::get);
    get_request.target("/api/GetRequestTest/404");
    get_request.version(11);
    get_request.prepare_payload();

    http::response<http::vector_body<char>> response = handler.handle_request(get_request);

    EXPECT_EQ(response.result(), http::status::not_found);
}

// (4) DELETE Tests

TEST_F(CrudHandlerTest, DeleteRequestSuccessTest) {
    std::vector<std::string> echo_paths;
    std::map<std::string, std::string> static_paths;
    std::map<std::string, std::string> crud_paths;
    echo_paths.push_back("/echo1");
    static_paths["/static"] = "./static";
    crud_paths["/api"] = "./data";
    ServerPaths sp;
    sp.echo_ = echo_paths;
    sp.static_ = static_paths;
    sp.crud_ = crud_paths;
    ConfigInterpreter::setServerPaths(sp);

    // First, need to create an object so we can delete it 
    std::string string_body = "{ \"data\": \"1\"}";
    std::vector<char> post_body(string_body.begin(),string_body.end());
    http::request<http::vector_body<char>> req;
    req.method(http::verb::post);
    req.target("/api/Books");
    req.version(11);
    req.body() = post_body;
    req.prepare_payload();

    std::shared_ptr<CrudFileManager> manager = std::make_shared<CrudFileManager>();;
    CrudHandler handler(manager);
    http::response<http::vector_body<char>> response = handler.handle_request(req);
    EXPECT_EQ(response.result(), http::status::created);

    // Now, let's delete this object 
    http::request<http::vector_body<char>> del_req;

    del_req.method(http::verb::delete_);
    del_req.target("/api/Books/1");
    del_req.version(11);
    del_req.prepare_payload();

    response = handler.handle_request(del_req);
    EXPECT_EQ(response.result(), http::status::ok);

    // Now, confirm if file deleted
    EXPECT_FALSE(std::filesystem::exists("./data/Books/1"));

    int deleted = std::filesystem::remove_all(crud_paths["/api"] );
}

TEST_F(CrudHandlerTest, DeleteRequestFailNoIDTest) {
    std::vector<std::string> echo_paths;
    std::map<std::string, std::string> static_paths;
    std::map<std::string, std::string> crud_paths;
    echo_paths.push_back("/echo1");
    static_paths["/static"] = "./static";
    crud_paths["/api"] = "./data";
    ServerPaths sp;
    sp.echo_ = echo_paths;
    sp.static_ = static_paths;
    sp.crud_ = crud_paths;
    ConfigInterpreter::setServerPaths(sp);

    // First, need to create an object so we can delete it 
    std::string string_body = "{ \"data\": \"1\"}";
    std::vector<char> post_body(string_body.begin(),string_body.end());
    http::request<http::vector_body<char>> req;
    req.method(http::verb::post);
    req.target("/api/Books");
    req.version(11);
    req.body() = post_body;
    req.prepare_payload();

    std::shared_ptr<CrudFileManager> manager = std::make_shared<CrudFileManager>();;
    CrudHandler handler(manager);
    http::response<http::vector_body<char>> response = handler.handle_request(req);
    EXPECT_EQ(response.result(), http::status::created);

    // Now, let's delete this object 
    req.method(http::verb::delete_);
    req.target("/api/Books");
    req.version(11);
    req.prepare_payload();

    response = handler.handle_request(req);
    EXPECT_EQ(response.result(), http::status::bad_request);

    int deleted = std::filesystem::remove_all(crud_paths["/api"] );
}

TEST_F(CrudHandlerTest, DeleteRequestFailNoFileTest) {
    std::vector<std::string> echo_paths;
    std::map<std::string, std::string> static_paths;
    std::map<std::string, std::string> crud_paths;
    echo_paths.push_back("/echo1");
    static_paths["/static"] = "./static";
    crud_paths["/api"] = "./data";
    ServerPaths sp;
    sp.echo_ = echo_paths;
    sp.static_ = static_paths;
    sp.crud_ = crud_paths;
    ConfigInterpreter::setServerPaths(sp);

    // Let's delete a non-existent object
    http::request<http::vector_body<char>> req;
    req.method(http::verb::delete_);
    req.target("/api/Books/1");
    req.version(11);
    req.prepare_payload();

    std::shared_ptr<CrudFileManager> manager = std::make_shared<CrudFileManager>();;
    CrudHandler handler(manager);
    http::response<http::vector_body<char>> response = handler.handle_request(req);
    EXPECT_EQ(response.result(), http::status::no_content);

    int deleted = std::filesystem::remove_all(crud_paths["/api"] );
}

// (5) LIST Tests

TEST_F(CrudHandlerTest, ListRequestPassNoFilesTest) {
    std::vector<std::string> echo_paths;
    std::map<std::string, std::string> static_paths;
    std::map<std::string, std::string> crud_paths;
    echo_paths.push_back("/echo1");
    static_paths["/static"] = "./static";
    crud_paths["/api"] = "./data";
    ServerPaths sp;
    sp.echo_ = echo_paths;
    sp.static_ = static_paths;
    sp.crud_ = crud_paths;
    ConfigInterpreter::setServerPaths(sp);

    std::string string_body = "";
    std::vector<char> list_body(string_body.begin(),string_body.end());
    http::request<http::vector_body<char>> list_request;
    list_request.method(http::verb::get);
    list_request.target("/api/ListRequestTest");
    list_request.version(11);
    list_request.body() = list_body;
    list_request.prepare_payload();

    std::shared_ptr<CrudFileManager> manager = std::make_shared<CrudFileManager>();;
    CrudHandler handler(manager);
    http::response<http::vector_body<char>> response = handler.handle_request(list_request);
    EXPECT_EQ(response.result(), http::status::ok);

    std::string response_str(response.body().data(), response.body().size()); 
    EXPECT_EQ(response_str, "[]"); 

    int deleted = std::filesystem::remove_all(crud_paths["/api"] );
}

TEST_F(CrudHandlerTest, ListRequestPassOneFileTest) {
    std::vector<std::string> echo_paths;
    std::map<std::string, std::string> static_paths;
    std::map<std::string, std::string> crud_paths;
    echo_paths.push_back("/echo1");
    static_paths["/static"] = "./static";
    crud_paths["/api"] = "./data";
    ServerPaths sp;
    sp.echo_ = echo_paths;
    sp.static_ = static_paths;
    sp.crud_ = crud_paths;
    ConfigInterpreter::setServerPaths(sp);

    // First, need to create an object so we can update it (should have ID 1)
    std::string string_body = "{ \"data\": \"1\"}";
    std::vector<char> post_body(string_body.begin(),string_body.end());
    http::request<http::vector_body<char>> req;
    req.method(http::verb::post);
    req.target("/api/ListRequestTest");
    req.version(11);
    req.body() = post_body;
    req.prepare_payload();

     std::shared_ptr<CrudFileManager> manager = std::make_shared<CrudFileManager>();;
    CrudHandler handler(manager);
    http::response<http::vector_body<char>> response = handler.handle_request(req);
    EXPECT_EQ(response.result(), http::status::created);

    // Call the list GET request
    string_body = "";
    std::vector<char> list_body(string_body.begin(),string_body.end());
    http::request<http::vector_body<char>> list_request;
    list_request.method(http::verb::get);
    list_request.target("/api/ListRequestTest");
    list_request.version(11);
    list_request.body() = list_body;
    list_request.prepare_payload();

    response = handler.handle_request(list_request);
    EXPECT_EQ(response.result(), http::status::ok);

    std::string response_str(response.body().data(), response.body().size()); 
    EXPECT_EQ(response_str, "[1]"); 

    int deleted = std::filesystem::remove_all(crud_paths["/api"] );
}

TEST_F(CrudHandlerTest, ListRequestPassMultipleFilesTest) {
    std::vector<std::string> echo_paths;
    std::map<std::string, std::string> static_paths;
    std::map<std::string, std::string> crud_paths;
    echo_paths.push_back("/echo1");
    static_paths["/static"] = "./static";
    crud_paths["/api"] = "./data";
    ServerPaths sp;
    sp.echo_ = echo_paths;
    sp.static_ = static_paths;
    sp.crud_ = crud_paths;
    ConfigInterpreter::setServerPaths(sp);

    // First, need to create an object so we can update it (should have ID 1)
    std::string string_body = "{ \"data\": \"1\"}";
    std::vector<char> post_body(string_body.begin(),string_body.end());
    http::request<http::vector_body<char>> req;
    req.method(http::verb::post);
    req.target("/api/ListRequestTest");
    req.version(11);
    req.body() = post_body;
    req.prepare_payload();

    std::shared_ptr<CrudFileManager> manager = std::make_shared<CrudFileManager>();;
    CrudHandler handler(manager);
    http::response<http::vector_body<char>> response = handler.handle_request(req);
    EXPECT_EQ(response.result(), http::status::created);

    // Create another object (should have ID 2)
    response = handler.handle_request(req);
    EXPECT_EQ(response.result(), http::status::created);

    // Create another object (should have ID 3)
    response = handler.handle_request(req);
    EXPECT_EQ(response.result(), http::status::created);

    // Call the list GET request
    string_body = "";
    std::vector<char> list_body(string_body.begin(),string_body.end());
    http::request<http::vector_body<char>> list_request;
    list_request.method(http::verb::get);
    list_request.target("/api/ListRequestTest");
    list_request.version(11);
    list_request.body() = list_body;
    list_request.prepare_payload();

    response = handler.handle_request(list_request);
    EXPECT_EQ(response.result(), http::status::ok);

    std::string response_str(response.body().data(), response.body().size()); 
    EXPECT_EQ(response_str, "[1, 2, 3]"); 

    int deleted = std::filesystem::remove_all(crud_paths["/api"] );
}

// (6) Miscellaneous Tests 

TEST_F(CrudHandlerTest, Factory) {
    auto factory_genereated_handler = CrudHandlerFactory::create();
    EXPECT_TRUE(factory_genereated_handler != nullptr);
}

TEST_F(CrudHandlerTest, BadRequestMethodTest) {
    http::request<http::vector_body<char>> req;
    req.method(http::verb::connect);
    req.target("/api/Books");
    req.version(11);

    http::response<http::vector_body<char>> response = handler->handle_request(req);

    EXPECT_EQ(response.result(), http::status::bad_request);
}
