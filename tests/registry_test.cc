#include <gtest/gtest.h>
#include "registry.h"
#include "echo_handler.h"
#include "static_handler.h"
#include "not_found_handler.h"
#include "logging_buffer.h"
#include "queue"

TEST(RegistryTest, RequestHandlers){
    std::queue<BufferEntry> q1;
    std::queue<BufferEntry> q2;
    LoggingBuffer lb(&q1, &q2);

    Registry::RegisterRequestHandler(Echo, EchoHandlerFactory::create);
    Registry::RegisterRequestHandler(Static, StaticHandlerFactory::create);
    Registry::RegisterRequestHandler(None, NotFoundHandlerFactory::create);

    EXPECT_TRUE(Registry::GetRequestHandler(Echo, &lb) != nullptr);
    EXPECT_TRUE(Registry::GetRequestHandler(Static, &lb) != nullptr);
    EXPECT_TRUE(Registry::GetRequestHandler(None, &lb) != nullptr);
}

// Nullptr test
TEST(RegistryTest, GetRequestHandlerNonExistentType) {
    std::queue<BufferEntry> q1;
    std::queue<BufferEntry> q2;
    LoggingBuffer lb(&q1, &q2);

    RequestHandler* handler = Registry::GetRequestHandler(BogusType, &lb);
    EXPECT_EQ(handler, nullptr);
}