#include "gtest/gtest.h"
#include "config_interpreter.h"
#include <boost/log/trivial.hpp>


class NginxConfigInterpreterTest : public testing::Test {
  protected:
    NginxConfig* config;

};

//If there is no port, should return 80
TEST_F(NginxConfigInterpreterTest, PortNotFound) {
    //Setup
    config = new NginxConfig;

    //Execution
    int port = ConfigInterpreter::getPort(*config);
    
    //Assertion
    EXPECT_EQ(port, 80);
}

//If there is a set port, it should return the set port
TEST_F(NginxConfigInterpreterTest, CorrectPortFound) {
    //Setup
    const std::string expected_port = "45";
    config = new NginxConfig;
    NginxConfigStatement* server_statement = new NginxConfigStatement;
    config->statements_.emplace_back(server_statement);
    server_statement->tokens_.emplace_back("server");
    NginxConfig* server_config = new NginxConfig;
    server_statement->child_block_.reset(server_config);
    NginxConfigStatement* port_statement = new NginxConfigStatement;
    server_config->statements_.emplace_back(port_statement);
    port_statement->tokens_.emplace_back("listen");
    port_statement->tokens_.emplace_back(expected_port);

    //Execution
    int port = ConfigInterpreter::getPort(*config);
    
    //Assertion
    EXPECT_EQ(port, std::stoi(expected_port));
}

//Out of range (for port values) port, should set to 80
TEST_F(NginxConfigInterpreterTest, OutOfRangePort) {
    //Setup
    const std::string input_port = "9999999";
    const int expected_port = 80;
    config = new NginxConfig;
    NginxConfigStatement* server_statement = new NginxConfigStatement;
    config->statements_.emplace_back(server_statement);
    server_statement->tokens_.emplace_back("server");
    NginxConfig* server_config = new NginxConfig;
    server_statement->child_block_.reset(server_config);
    NginxConfigStatement* port_statement = new NginxConfigStatement;
    server_config->statements_.emplace_back(port_statement);
    port_statement->tokens_.emplace_back("listen");
    port_statement->tokens_.emplace_back(input_port);

    //Execution
    int port = ConfigInterpreter::getPort(*config);
    
    //Assertion
    EXPECT_EQ(port, expected_port);
}

//Out of range (integer) port, should set to 80
TEST_F(NginxConfigInterpreterTest, OutOfRangeInt) {
    //Setup
    const std::string input_port = "99999999999999999999999";
    const int expected_port = 80;
    config = new NginxConfig;
    NginxConfigStatement* server_statement = new NginxConfigStatement;
    config->statements_.emplace_back(server_statement);
    server_statement->tokens_.emplace_back("server");
    NginxConfig* server_config = new NginxConfig;
    server_statement->child_block_.reset(server_config);
    NginxConfigStatement* port_statement = new NginxConfigStatement;
    server_config->statements_.emplace_back(port_statement);
    port_statement->tokens_.emplace_back("listen");
    port_statement->tokens_.emplace_back(input_port);

    //Execution
    int port = ConfigInterpreter::getPort(*config);
    
    //Assertion
    EXPECT_EQ(port, expected_port);
}

//Non integer port should set to 80
TEST_F(NginxConfigInterpreterTest, NonIntegerPort) {
    //Setup
    const std::string input_port = "foo";
    const int expected_port = 80;
    config = new NginxConfig;
    NginxConfigStatement* server_statement = new NginxConfigStatement;
    config->statements_.emplace_back(server_statement);
    server_statement->tokens_.emplace_back("server");
    NginxConfig* server_config = new NginxConfig;
    server_statement->child_block_.reset(server_config);
    NginxConfigStatement* port_statement = new NginxConfigStatement;
    server_config->statements_.emplace_back(port_statement);
    port_statement->tokens_.emplace_back("listen");
    port_statement->tokens_.emplace_back(input_port);

    //Execution
    int port = ConfigInterpreter::getPort(*config);
    
    //Assertion
    EXPECT_EQ(port, expected_port);
}

// // Tests for getServerPaths

// Correct paths found
TEST_F(NginxConfigInterpreterTest, CorrectPathsFound) {
    // Setup
    config = new NginxConfig;
    NginxConfigStatement* server_statement = new NginxConfigStatement;
    config->statements_.emplace_back(server_statement);
    server_statement->tokens_.emplace_back("server");

    // Create server block
    NginxConfig* server_config = new NginxConfig;
    server_statement->child_block_.reset(server_config);

    // Create static location block
    NginxConfigStatement* static_location_statement = new NginxConfigStatement;
    server_config->statements_.emplace_back(static_location_statement);
    static_location_statement->tokens_.emplace_back("location");
    static_location_statement->tokens_.emplace_back("/staticblah");
    static_location_statement->tokens_.emplace_back("StaticHandler");
    NginxConfig* static_location_config = new NginxConfig;
    static_location_statement->child_block_.reset(static_location_config);
    NginxConfigStatement* static_root_statement = new NginxConfigStatement;
    static_location_config->statements_.emplace_back(static_root_statement);
    static_root_statement->tokens_.emplace_back("root");
    static_root_statement->tokens_.emplace_back("/blah");



    // Create echo location block
    NginxConfigStatement* echo_location_statement = new NginxConfigStatement;
    server_config->statements_.emplace_back(echo_location_statement);
    echo_location_statement->tokens_.emplace_back("location");
    echo_location_statement->tokens_.emplace_back("/echo");
    echo_location_statement->tokens_.emplace_back("EchoHandler");
    NginxConfig* echo_location_config = new NginxConfig;
    echo_location_statement->child_block_.reset(echo_location_config);


    // Execution
    ConfigInterpreter::setServerPaths(*config);
    ServerPaths server_paths = ConfigInterpreter::getServerPaths();
    
    // Assertion
    ASSERT_EQ(server_paths.echo_.size(), 1);
    EXPECT_EQ(server_paths.echo_[0], "/echo");
    ASSERT_EQ(server_paths.static_.size(), 1);
    EXPECT_EQ(server_paths.static_["/staticblah"], "/blah");
}

// No paths specified
TEST_F(NginxConfigInterpreterTest, NoPaths) {
    // Setup
    config = new NginxConfig;

    // Execution
    ConfigInterpreter::setServerPaths(*config);
    ServerPaths server_paths = ConfigInterpreter::getServerPaths();    

    // Assertion
    EXPECT_TRUE(server_paths.echo_.empty());
    EXPECT_TRUE(server_paths.static_.empty());
}

//Path specified twice, should throw runtime error
TEST_F(NginxConfigInterpreterTest, PathSpecifiedTwice) {
    // Setup
    config = new NginxConfig;
    NginxConfigStatement* server_statement = new NginxConfigStatement;
    config->statements_.emplace_back(server_statement);
    server_statement->tokens_.emplace_back("server");

    // Create server block
    NginxConfig* server_config = new NginxConfig;
    server_statement->child_block_.reset(server_config);

    // Create static location block
    NginxConfigStatement* static_location_statement = new NginxConfigStatement;
    server_config->statements_.emplace_back(static_location_statement);
    static_location_statement->tokens_.emplace_back("location");
    static_location_statement->tokens_.emplace_back("/same/path");
    static_location_statement->tokens_.emplace_back("StaticHandler");
    NginxConfig* static_location_config = new NginxConfig;
    static_location_statement->child_block_.reset(static_location_config);
    NginxConfigStatement* static_root_statement = new NginxConfigStatement;
    static_location_config->statements_.emplace_back(static_root_statement);
    static_root_statement->tokens_.emplace_back("root");
    static_root_statement->tokens_.emplace_back("/blah");



    // Create echo location block
    NginxConfigStatement* echo_location_statement = new NginxConfigStatement;
    server_config->statements_.emplace_back(echo_location_statement);
    echo_location_statement->tokens_.emplace_back("location");
    echo_location_statement->tokens_.emplace_back("/same/path");
    echo_location_statement->tokens_.emplace_back("EchoHandler");
    NginxConfig* echo_location_config = new NginxConfig;
    echo_location_statement->child_block_.reset(echo_location_config);


    // Execution
    try{
        ConfigInterpreter::setServerPaths(*config);
        ServerPaths server_paths = ConfigInterpreter::getServerPaths();    }
    //Expect
    catch(std::runtime_error const & err){
        BOOST_LOG_TRIVIAL(info) << "Error: "<< err.what();
        EXPECT_EQ(err.what(), std::string("The same location (/same/path) is specified for multiple handlers"));
    }
}


//Root specified twice, should throw runtime error
TEST_F(NginxConfigInterpreterTest, RootSpecifiedTwice) {
    // Setup
    config = new NginxConfig;
    NginxConfigStatement* server_statement = new NginxConfigStatement;
    config->statements_.emplace_back(server_statement);
    server_statement->tokens_.emplace_back("server");

    // Create server block
    NginxConfig* server_config = new NginxConfig;
    server_statement->child_block_.reset(server_config);

    // Create static location block
    NginxConfigStatement* static_location_statement = new NginxConfigStatement;
    server_config->statements_.emplace_back(static_location_statement);
    static_location_statement->tokens_.emplace_back("location");
    static_location_statement->tokens_.emplace_back("/same/path");
    static_location_statement->tokens_.emplace_back("StaticHandler");
    NginxConfig* static_location_config = new NginxConfig;
    static_location_statement->child_block_.reset(static_location_config);
    NginxConfigStatement* static_root_statement = new NginxConfigStatement;
    static_location_config->statements_.emplace_back(static_root_statement);
    static_root_statement->tokens_.emplace_back("root");
    static_root_statement->tokens_.emplace_back("/blah");
    NginxConfigStatement* second_static_root_statement = new NginxConfigStatement;
    static_location_config->statements_.emplace_back(static_root_statement);
    static_root_statement->tokens_.emplace_back("root");
    static_root_statement->tokens_.emplace_back("/dsfsd");



    // Execution
    try{
        ConfigInterpreter::setServerPaths(*config);
        ServerPaths server_paths = ConfigInterpreter::getServerPaths();
    }
    //Expect
    catch(std::runtime_error const & err){
        BOOST_LOG_TRIVIAL(info) << "Error: "<< err.what();
        EXPECT_EQ(err.what(), std::string("Duplicate server file location"));
    }
}


TEST_F(NginxConfigInterpreterTest, CrudHandlerTest) {
    config = new NginxConfig;
    std::map<std::string, std::string> crud_paths_to_server_paths;
    std::string location = "/api/crud";

    // Mock a statement representing the configuration block
    NginxConfigStatement* lv3statement = new NginxConfigStatement;
    lv3statement->tokens_ = {"data_path", "/data/crud"};

    NginxConfig* child_block = new NginxConfig;
    child_block->statements_.emplace_back(lv3statement);

    NginxConfigStatement* lv2Statement = new NginxConfigStatement;
    lv2Statement->child_block_.reset(child_block);

    bool found_server_file = false;
    for (const auto& lv3statement : lv2Statement->child_block_->statements_) {
        if (lv3statement->tokens_.at(0) == "data_path") {
            if (found_server_file) {
                BOOST_LOG_TRIVIAL(fatal) << "Duplicate server file location";
                throw std::runtime_error("Duplicate server file location");
            }
            BOOST_LOG_TRIVIAL(info) << "Adding crud path: " << location << " mapping to server location " << lv3statement->tokens_.at(1);
            crud_paths_to_server_paths[location] = lv3statement->tokens_.at(1);
            found_server_file = true;
        }
    }

    EXPECT_TRUE(found_server_file);
    EXPECT_EQ(crud_paths_to_server_paths[location], "/data/crud");
}

TEST_F(NginxConfigInterpreterTest, DuplicateCrudHandlerTest) {
    config = new NginxConfig;
    std::map<std::string, std::string> crud_paths_to_server_paths;
    std::string location = "/api/crud";

    // Mock duplicate statements representing the configuration block
    NginxConfigStatement* lv3statement1 = new NginxConfigStatement;
    lv3statement1->tokens_ = {"data_path", "/data/crud"};

    NginxConfigStatement* lv3statement2 = new NginxConfigStatement;
    lv3statement2->tokens_ = {"data_path", "/data/crud"};

    NginxConfig* child_block = new NginxConfig;
    child_block->statements_.emplace_back(lv3statement1);
    child_block->statements_.emplace_back(lv3statement2);

    NginxConfigStatement* lv2Statement = new NginxConfigStatement;
    lv2Statement->child_block_.reset(child_block);

    bool found_server_file = false;
    EXPECT_THROW({
        for (const auto& lv3statement : lv2Statement->child_block_->statements_) {
            if (lv3statement->tokens_.at(0) == "data_path") {
                if (found_server_file) {
                    BOOST_LOG_TRIVIAL(fatal) << "Duplicate server file location";
                    throw std::runtime_error("Duplicate server file location");
                }
                BOOST_LOG_TRIVIAL(info) << "Adding crud path: " << location << " mapping to server location " << lv3statement->tokens_.at(1);
                crud_paths_to_server_paths[location] = lv3statement->tokens_.at(1);
                found_server_file = true;
            }
        }
    }, std::runtime_error);
}

TEST_F(NginxConfigInterpreterTest, HealthHandlerTest) {
    config = new NginxConfig;
    std::vector<std::string> health_paths;
    std::string location = "/api/health";

    BOOST_LOG_TRIVIAL(info) << "Adding health path: " << location;
    health_paths.push_back(location);

    EXPECT_EQ(health_paths.size(), 1);
    EXPECT_EQ(health_paths[0], location);
}

TEST_F(NginxConfigInterpreterTest, SleepHandlerTest) {
    config = new NginxConfig;
    std::vector<std::string> sleep_paths;
    std::string location = "/api/sleep";

    BOOST_LOG_TRIVIAL(info) << "Adding sleep path: " << location;
    sleep_paths.push_back(location);

    EXPECT_EQ(sleep_paths.size(), 1);
    EXPECT_EQ(sleep_paths[0], location);
}

TEST_F(NginxConfigInterpreterTest, CrudHandlerFound) {
  // Setup
  config = new NginxConfig;
  NginxConfigStatement* server_statement = new NginxConfigStatement;
  config->statements_.emplace_back(server_statement);
  server_statement->tokens_.emplace_back("server");

  // Create server block
  NginxConfig* server_config = new NginxConfig;
  server_statement->child_block_.reset(server_config);

  // Create CRUD location block with data_path
  NginxConfigStatement* crud_location_statement = new NginxConfigStatement;
  server_config->statements_.emplace_back(crud_location_statement);
  crud_location_statement->tokens_.emplace_back("location");
  crud_location_statement->tokens_.emplace_back("/api/data");
  crud_location_statement->tokens_.emplace_back("CrudHandler");
  NginxConfig* crud_location_config = new NginxConfig;
  crud_location_statement->child_block_.reset(crud_location_config);
  NginxConfigStatement* data_path_statement = new NginxConfigStatement;
  crud_location_config->statements_.emplace_back(data_path_statement);
  data_path_statement->tokens_.emplace_back("data_path");
  data_path_statement->tokens_.emplace_back("/data/crud");

  // Execution
  ConfigInterpreter::setServerPaths(*config);
  ServerPaths server_paths = ConfigInterpreter::getServerPaths();

  // Assertion
  EXPECT_EQ(server_paths.crud_.size(), 1);
  EXPECT_EQ(server_paths.crud_["/api/data"], "/data/crud");
}

// Test for not finding a CRUD handler (no data_path)
TEST_F(NginxConfigInterpreterTest, CrudHandlerNotFound) {
  // Setup
  config = new NginxConfig;
  NginxConfigStatement* server_statement = new NginxConfigStatement;
  config->statements_.emplace_back(server_statement);
  server_statement->tokens_.emplace_back("server");

  // Create server block
  NginxConfig* server_config = new NginxConfig;
  server_statement->child_block_.reset(server_config);

  // Create location block with type CrudHandler but no data_path
  NginxConfigStatement* crud_location_statement = new NginxConfigStatement;
  server_config->statements_.emplace_back(crud_location_statement);
  crud_location_statement->tokens_.emplace_back("location");
  crud_location_statement->tokens_.emplace_back("/api/data");
  crud_location_statement->tokens_.emplace_back("CrudHandler");
  NginxConfig* crud_location_config = new NginxConfig;
  crud_location_statement->child_block_.reset(crud_location_config);

  // Execution
  ConfigInterpreter::setServerPaths(*config);
  ServerPaths server_paths = ConfigInterpreter::getServerPaths();

  // Assertion
  EXPECT_EQ(server_paths.crud_.size(), 0);
}

// Test for finding multiple CRUD handlers
TEST_F(NginxConfigInterpreterTest, MultipleCrudHandlers) {
  // Setup
  config = new NginxConfig;
  NginxConfigStatement* server_statement = new NginxConfigStatement;
  config->statements_.emplace_back(server_statement);
  server_statement->tokens_.emplace_back("server");

  // Create server block
  NginxConfig* server_config = new NginxConfig;
  server_statement->child_block_.reset(server_config);

  // Create first CRUD location block with data_path
  NginxConfigStatement* crud_location_statement1 = new NginxConfigStatement;
  server_config->statements_.emplace_back(crud_location_statement1);
  crud_location_statement1->tokens_.emplace_back("location");
  crud_location_statement1->tokens_.emplace_back("/api/data1");
  crud_location_statement1->tokens_.emplace_back("CrudHandler");
  NginxConfig* crud_location_config1 = new NginxConfig;
  crud_location_statement1->child_block_.reset(crud_location_config1);
  NginxConfigStatement* data_path_statement1 = new NginxConfigStatement;
  crud_location_config1->statements_.emplace_back(data_path_statement1);
  data_path_statement1->tokens_.emplace_back("data_path");
  data_path_statement1->tokens_.emplace_back("/data/crud1");

  // Create second CRUD location block with data_path
  NginxConfigStatement* crud_location_statement2 = new NginxConfigStatement;
  server_config->statements_.emplace_back(crud_location_statement2);
  crud_location_statement2->tokens_.emplace_back("location");
  crud_location_statement2->tokens_.emplace_back("/api/data2");
  crud_location_statement2->tokens_.emplace_back("CrudHandler");
  NginxConfig* crud_location_config2 = new NginxConfig;
  crud_location_statement2->child_block_.reset(crud_location_config2);
  NginxConfigStatement* data_path_statement2 = new NginxConfigStatement;
  crud_location_config2->statements_.emplace_back(data_path_statement2);
  data_path_statement2->tokens_.emplace_back("data_path");
  data_path_statement2->tokens_.emplace_back("/data/crud2");

  // Execution
  ConfigInterpreter::setServerPaths(*config);
  ServerPaths server_paths = ConfigInterpreter::getServerPaths();

  // Assertion
  EXPECT_EQ(server_paths.crud_.size(), 2);
  EXPECT_EQ(server_paths.crud_["/api/data1"], "/data/crud1");
  EXPECT_EQ(server_paths.crud_["/api/data2"], "/data/crud2");
}


// Test for finding multiple CRUD handlers, health handlers and sleep handlers
TEST_F(NginxConfigInterpreterTest, MultipleHandlers) {
  // Setup
  config = new NginxConfig;
  NginxConfigStatement* server_statement = new NginxConfigStatement;
  config->statements_.emplace_back(server_statement);
  server_statement->tokens_.emplace_back("server");

  // Create server block
  NginxConfig* server_config = new NginxConfig;
  server_statement->child_block_.reset(server_config);

  // Create first CRUD location block with data_path
  NginxConfigStatement* crud_location_statement1 = new NginxConfigStatement;
  server_config->statements_.emplace_back(crud_location_statement1);
  crud_location_statement1->tokens_.emplace_back("location");
  crud_location_statement1->tokens_.emplace_back("/api/data1");
  crud_location_statement1->tokens_.emplace_back("CrudHandler");
  NginxConfig* crud_location_config1 = new NginxConfig;
  crud_location_statement1->child_block_.reset(crud_location_config1);
  NginxConfigStatement* data_path_statement1 = new NginxConfigStatement;
  crud_location_config1->statements_.emplace_back(data_path_statement1);
  data_path_statement1->tokens_.emplace_back("data_path");
  data_path_statement1->tokens_.emplace_back("/data/crud1");

  // Create second CRUD location block with data_path
  NginxConfigStatement* crud_location_statement2 = new NginxConfigStatement;
  server_config->statements_.emplace_back(crud_location_statement2);
  crud_location_statement2->tokens_.emplace_back("location");
  crud_location_statement2->tokens_.emplace_back("/api/data2");
  crud_location_statement2->tokens_.emplace_back("CrudHandler");
  NginxConfig* crud_location_config2 = new NginxConfig;
  crud_location_statement2->child_block_.reset(crud_location_config2);
  NginxConfigStatement* data_path_statement2 = new NginxConfigStatement;
  crud_location_config2->statements_.emplace_back(data_path_statement2);
  data_path_statement2->tokens_.emplace_back("data_path");
  data_path_statement2->tokens_.emplace_back("/data/crud2");

  // Create Health Handler location
  NginxConfigStatement* health_location_statement = new NginxConfigStatement;
  server_config->statements_.emplace_back(health_location_statement);
  health_location_statement->tokens_.emplace_back("location");
  health_location_statement->tokens_.emplace_back("/health");
  health_location_statement->tokens_.emplace_back("HealthHandler");

  // Create Sleep Handler location
  NginxConfigStatement* sleep_location_statement = new NginxConfigStatement;
  server_config->statements_.emplace_back(sleep_location_statement);
  sleep_location_statement->tokens_.emplace_back("location");
  sleep_location_statement->tokens_.emplace_back("/sleep");
  sleep_location_statement->tokens_.emplace_back("SleepHandler");

  // Execution
  ConfigInterpreter::setServerPaths(*config);
  ServerPaths server_paths = ConfigInterpreter::getServerPaths();

  // Assertion
  EXPECT_EQ(server_paths.crud_.size(), 2);
  EXPECT_EQ(server_paths.crud_["/api/data1"], "/data/crud1");
  EXPECT_EQ(server_paths.crud_["/api/data2"], "/data/crud2");
  EXPECT_EQ(server_paths.health_.size(), 1);
  EXPECT_EQ(server_paths.health_[0], "/health");
  EXPECT_EQ(server_paths.sleep_.size(), 1);
  EXPECT_EQ(server_paths.sleep_[0], "/sleep");
}
