#include "gtest/gtest.h"
#include "config_parser.h"


class NginxConfigParserTest : public testing::Test {
  protected:
    NginxConfigParser parser;
    NginxConfig out_config;

};

TEST_F(NginxConfigParserTest, SimpleConfig) {
  bool success = parser.Parse("configs/example_config", &out_config);

  EXPECT_TRUE(success);
}


//File not found
//Expect FALSE
TEST_F(NginxConfigParserTest, FileNotFound){
  bool success = parser.Parse("configs/file_not_found", &out_config);

  EXPECT_FALSE(success);
}

//Empty config
//Expect FALSE
TEST_F(NginxConfigParserTest, EmptyConfig){
  bool success = parser.Parse("configs/empty_config", &out_config);

  EXPECT_FALSE(success);
}

//Config with comment
//Expect TRUE
TEST_F(NginxConfigParserTest, ConfigWithComment){
  bool success = parser.Parse("configs/config_with_comment", &out_config);

  EXPECT_TRUE(success);
}

//events resides in main context
//Expect TRUE
TEST_F(NginxConfigParserTest, EventsResideMainContext){
  bool success = parser.Parse("configs/events_reside_main_context", &out_config);

  EXPECT_TRUE(success);
}

//http resides in the main context
//Expect TRUE
TEST_F(NginxConfigParserTest, HttpResideMainContext){
  bool success = parser.Parse("configs/http_resides_main_context", &out_config);

  EXPECT_TRUE(success);
}

//server resides in the http context
//Expect TRUE
TEST_F(NginxConfigParserTest, ServerResidesHttpContext){
  bool success = parser.Parse("configs/server_resides_http_context", &out_config);

  EXPECT_TRUE(success);
}

//location resides in the server context
//Expect TRUE
TEST_F(NginxConfigParserTest, LocationResidesServerContext){
  bool success = parser.Parse("configs/location_resides_server_context", &out_config);

  EXPECT_TRUE(success);
}

//Back to back end brackets }
//Expect True
TEST_F(NginxConfigParserTest, MultipleEndBrackets){
  bool success = parser.Parse("configs/multiple_end_brackets", &out_config);

  EXPECT_TRUE(success);
}

//Empty brackets
//Expect true
TEST_F(NginxConfigParserTest, EmptyBrackets){
  bool success = parser.Parse("configs/empty_brackets", &out_config);

  EXPECT_TRUE(success);
}

//Missing close bracket
//Expect FALSE
TEST_F(NginxConfigParserTest, MissingClosingBracket){
  bool success = parser.Parse("configs/missing_closing_bracket", &out_config);

  EXPECT_FALSE(success);
}


//Test  to_string for NginxConfigStatement
TEST_F(NginxConfigParserTest, NginxConfigStatementToString){

    //Setup
    NginxConfigStatement* port_statement = new NginxConfigStatement;
    port_statement->tokens_.emplace_back("listen");
    port_statement->tokens_.emplace_back("80");

    //Execute
    std::string str = port_statement->ToString(100);

    //Validate
    EXPECT_TRUE(str.find("listen 80;") != std::string::npos);

}

// Tests that the parser can handle empty statements properly.
// TEST_F(NginxConfigParserTest, HandlesEmptyStatement) {
//   bool success = parser.Parse("configs/empty_statement_config", &out_config);
//   EXPECT_FALSE(success);
// }

// Tests that the parser can handle quoted strings.
TEST_F(NginxConfigParserTest, HandlesQuotedString) {
  bool success = parser.Parse("configs/quoted_string_config", &out_config);
  EXPECT_TRUE(success);
}

// Tests that the parser returns false for unclosed quotes.
TEST_F(NginxConfigParserTest, HandlesUnclosedQuotes) {
  bool success = parser.Parse("configs/unclosed_quote_config", &out_config);
  EXPECT_FALSE(success);
}

// Tests that the parser handles invalid characters.
// TEST_F(NginxConfigParserTest, HandlesInvalidCharacters) {
//   bool success = parser.Parse("configs/invalid_char_config", &out_config);
//   EXPECT_FALSE(success);
// }

// Tests that the parser returns false for unexpected EOF.
TEST_F(NginxConfigParserTest, HandlesUnexpectedEOF) {
  std::istringstream config_stream("server { listen 80");
  bool success = parser.Parse(&config_stream, &out_config);
  EXPECT_FALSE(success);
}

// Tests that the parser can handle nested blocks properly.
TEST_F(NginxConfigParserTest, HandlesNestedBlocks) {
  bool success = parser.Parse("configs/nested_blocks_config", &out_config);
  EXPECT_TRUE(success);
}


