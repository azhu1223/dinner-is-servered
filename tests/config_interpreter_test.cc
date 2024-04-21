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