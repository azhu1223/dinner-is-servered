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



class CrudHandlerTest : public ::testing::Test {
protected:
    void SetUp() override {
        handler = new CrudHandler(CrudFileManager());
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
    req.target("/api/Books");
    req.version(11);
    req.body() = post_body;
    req.prepare_payload();

    CrudFileManager manager;
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
    req.target("/api/Books/1");
    req.version(11);
    req.body() = post_body;
    req.prepare_payload();

    CrudFileManager manager;
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

    CrudFileManager manager;
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

    // Try to update an object that doesn't exist 
    std::string string_body = "{ \"data\": \"1\"}";
    std::vector<char> put_body(string_body.begin(),string_body.end());
    http::request<http::vector_body<char>> req;
    req.method(http::verb::put);
    req.target("/api/Books/1");
    req.version(11);
    req.body() = put_body;
    req.prepare_payload();

    CrudFileManager manager;
    CrudHandler handler(manager);
    http::response<http::vector_body<char>> response = handler.handle_request(req);
    EXPECT_EQ(response.result(), http::status::not_found);
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

    CrudFileManager manager;
    CrudHandler handler(manager);
    http::response<http::vector_body<char>> response = handler.handle_request(req);
    EXPECT_EQ(response.result(), http::status::bad_request);
}

TEST_F(CrudHandlerTest, Factory) {
    auto factory_genereated_handler = CrudHandlerFactory::create();
    EXPECT_TRUE(factory_genereated_handler != nullptr);
}

// (3) Miscellaneous Tests 

TEST_F(CrudHandlerTest, BadRequestMethodTest) {
    http::request<http::vector_body<char>> req;
    req.method(http::verb::connect);
    req.target("/api/Books");
    req.version(11);

    http::response<http::vector_body<char>> response = handler->handle_request(req);

    EXPECT_EQ(response.result(), http::status::bad_request);
}
