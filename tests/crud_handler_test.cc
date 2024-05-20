#include <gtest/gtest.h>
#include "crud_handler.h"
#include <string>
#include <vector>
#include <fstream>
#include "config_interpreter.h"
#include <gmock/gmock.h>
#include <boost/beast/http.hpp>
#include "request_dispatcher.h"
#include "crud_file_manager.h"
#include <unordered_map>
namespace http = boost::beast::http;

class FakeCrudFileManager : public CrudFileManager {
  public:
    bool writeObject(CrudPath path, std::string json);
  private:
    std::unordered_map<std::string,std::string> filesystem;
};

bool FakeCrudFileManager::writeObject(CrudPath path, std::string json) {
  std::string file_path = path.entity_name + "/" + path.entity_id;
  filesystem[file_path] = json;
  return true;
}

class CrudHandlerTest : public ::testing::Test {
protected:
    void SetUp() override {
        handler = new CrudHandler(FakeCrudFileManager());
    }

    void TearDown() override {
        delete handler;
    }

    CrudHandler* handler;
};

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

    FakeCrudFileManager manager;
    CrudHandler handler(manager);
    http::response<http::vector_body<char>> response = handler.handle_request(req);

    EXPECT_EQ(response.result(), http::status::created);
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

    FakeCrudFileManager manager;
    CrudHandler handler(manager);
    http::response<http::vector_body<char>> response = handler.handle_request(req);

    EXPECT_EQ(response.result(), http::status::bad_request);
}

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
