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
    int port = getPort(*config);
    
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
    int port = getPort(*config);
    
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
    int port = getPort(*config);
    
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
    int port = getPort(*config);
    
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
    int port = getPort(*config);
    
    //Assertion
    EXPECT_EQ(port, expected_port);
}

// Tests for getServerPaths

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
    ServerPaths server_paths = getServerPaths(*config);
    
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
    ServerPaths server_paths = getServerPaths(*config);
    
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
        ServerPaths server_paths = getServerPaths(*config);
    }
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
        ServerPaths server_paths = getServerPaths(*config);
    }
    //Expect
    catch(std::runtime_error const & err){
        BOOST_LOG_TRIVIAL(info) << "Error: "<< err.what();
        EXPECT_EQ(err.what(), std::string("Duplicate server file location"));
    }
}