#include "request_parser.h"
#include "gtest/gtest.h"

const short kBytesTransferred = 10;
const char kData[10] = "Test data";
ServerPaths kServerPaths; 


class ParserFixture : public ::testing::Test {
protected:
    RequestParser* req;
    ServerPaths kServerPaths;

    ServerPaths xServerPaths;
    

    void SetUp() override {
        kServerPaths.echo_.push_back("/test/echo1");
        kServerPaths.echo_.push_back("/test/echo2");
        kServerPaths.static_["/test/static1"] = "/test/static1";
        kServerPaths.static_["/test/static2"] = "/test/static2";

        xServerPaths.echo_.push_back("/test/echo");
        xServerPaths.static_["/test/static"] = "/test/static";

        req = new RequestParser(kBytesTransferred, kData, kServerPaths);
    }

        std::unique_ptr<RequestParser> createRequestParser(const char* data, short bytes_transferred) {
        return std::make_unique<RequestParser>(bytes_transferred, data, xServerPaths);
    }

    void TearDown() override {
        delete req;
    }
};


TEST_F(ParserFixture, ParserConstructor) {
    EXPECT_STREQ(req->getFilePath().c_str(), "");
}


TEST_F(ParserFixture, EchoRequest_ReturnsTrue) {
    const char* requestData = "GET /test/echo/something HTTP/1.1\r\nHost: foo.com\r\n\r\n";
    auto parser = createRequestParser(requestData, strlen(requestData));
    EXPECT_TRUE(parser->isRequestEcho());
}

TEST_F(ParserFixture, NonEchoRequest_ReturnsFalse) {
    const char* requestData = "GET /test/other/something HTTP/1.1\r\nHost: foo.com\r\n\r\n";
    auto parser = createRequestParser(requestData, strlen(requestData));
    EXPECT_FALSE(parser->isRequestEcho());
}

TEST_F(ParserFixture, StaticRequest_ReturnsTrue) {
    const char* requestData = "GET /test/static/image.png HTTP/1.1\r\nHost: example.com\r\n\r\n";
    auto parser = createRequestParser(requestData, strlen(requestData));
    EXPECT_TRUE(parser->isRequestStatic());
}

TEST_F(ParserFixture, NonStaticRequest_ReturnsFalse) {
    const char* requestData = "GET /test/api/data HTTP/1.1\r\nHost: example.com\r\n\r\n";
    auto parser = createRequestParser(requestData, strlen(requestData));
    EXPECT_FALSE(parser->isRequestStatic());
}

TEST_F(ParserFixture, EchoRequestWithConsecutiveSlashes_ReturnsTrue) {
    const char* requestData = "GET /test/echo//something HTTP/1.1\r\nHost: example.com\r\n\r\n";
    auto parser = createRequestParser(requestData, strlen(requestData));
    EXPECT_TRUE(parser->isRequestEcho());
}

TEST_F(ParserFixture, StaticRequestWithConsecutiveSlashes_ReturnsTrue) {
    const char* requestData = "GET /test/static//image.png HTTP/1.1\r\nHost: example.com\r\n\r\n";
    auto parser = createRequestParser(requestData, strlen(requestData));
    EXPECT_TRUE(parser->isRequestStatic());
}

TEST_F(ParserFixture, GetTarget) {
    const char* requestData = "GET /test/static//image.png HTTP/1.1\r\nHost: example.com\r\n\r\n";
    auto parser = createRequestParser(requestData, strlen(requestData));

    std::string expected_target = "/test/static//image.png";
    boost::beast::string_view target= parser->getTarget();
    EXPECT_TRUE(expected_target == std::string(target));
}
