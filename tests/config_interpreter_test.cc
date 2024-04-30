#include "gtest/gtest.h"
#include "config_interpreter.h"

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

    // Create paths block
    NginxConfigStatement* paths_statement = new NginxConfigStatement;
    server_config->statements_.emplace_back(paths_statement);
    paths_statement->tokens_.emplace_back("paths");

    // Create echo and static paths
    NginxConfig* paths_config = new NginxConfig;
    paths_statement->child_block_.reset(paths_config);

    // Echo path
    NginxConfigStatement* echo_statement = new NginxConfigStatement;
    paths_config->statements_.emplace_back(echo_statement);
    echo_statement->tokens_.emplace_back("echo");
    echo_statement->tokens_.emplace_back("/echo");

    // Static path
    NginxConfigStatement* static_statement = new NginxConfigStatement;
    paths_config->statements_.emplace_back(static_statement);
    static_statement->tokens_.emplace_back("static");
    static_statement->tokens_.emplace_back("/static");

    // Execution
    ServerPaths server_paths = getServerPaths(*config);
    
    // Assertion
    ASSERT_EQ(server_paths.echo_.size(), 1);
    EXPECT_EQ(server_paths.echo_[0], "/echo");
    ASSERT_EQ(server_paths.static_.size(), 1);
    EXPECT_EQ(server_paths.static_[0], "/static");
}

// No paths specified, should add root to echo paths
TEST_F(NginxConfigInterpreterTest, DefaultEchoPathWhenNoPaths) {
    // Setup
    config = new NginxConfig;

    // Execution
    ServerPaths server_paths = getServerPaths(*config);
    
    // Assertion
    ASSERT_EQ(server_paths.echo_.size(), 1);
    EXPECT_EQ(server_paths.echo_[0], "/");
    EXPECT_TRUE(server_paths.static_.empty());
}



