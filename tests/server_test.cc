#include "gtest/gtest.h"
#include "server.h"

class ServerTestFixture : public ::testing::Test
{
  protected:

    boost::asio::io_service io_service; 
    void SetUp() override 
    {
    }
};

TEST_F(ServerTestFixture, StartAccept)
{ 
  server test_server(io_service, 8080);
  bool success = test_server.start_accept();
  EXPECT_TRUE(success);
}
