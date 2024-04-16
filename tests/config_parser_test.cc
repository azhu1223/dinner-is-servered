#include "gtest/gtest.h"
#include "config_parser.h"

// Test for a simple, valid configuration
TEST(NginxConfigParserTest, SimpleConfig) {
    NginxConfigParser parser;
    NginxConfig out_config;

    bool success = parser.Parse("example_config", &out_config);
    EXPECT_TRUE(success);
}

// Test for an invalid configuration (e.g., missing closing brace)
// TEST(NginxConfigParserTest, InvalidConfig) {
//     NginxConfigParser parser;
//     NginxConfig out_config;

//     bool success = parser.Parse("/usr/src/projects/manavgbruin24-config-parser/tests/example_config_invalid", &out_config);

//     EXPECT_FALSE(success);
// }

// Test for nested configuration blocks
// TEST(NginxConfigParserTest, NestedConfig) {
//     NginxConfigParser parser;
//     NginxConfig out_config;

//     bool success = parser.Parse("example_config_nested", &out_config);
//     EXPECT_TRUE(success);
// }

// Test for empty configuration
TEST(NginxConfigParserTest, EmptyConfig) {
    NginxConfigParser parser;
    NginxConfig out_config;

    bool success = parser.Parse("example_config_empty", &out_config);
    EXPECT_TRUE(success);  // This expects that parsing an empty file is successful
}


