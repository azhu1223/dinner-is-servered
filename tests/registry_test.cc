#include <gtest/gtest.h>
#include "registry.h"
#include "echo_handler.h"
#include "static_handler.h"
#include "not_found_handler.h"

TEST(RegistryTest, RequestHandlers){
    Registry::RegisterRequestHandler(Echo, EchoHandlerFactory::create);
    Registry::RegisterRequestHandler(Static, StaticHandlerFactory::create);
    Registry::RegisterRequestHandler(None, NotFoundHandlerFactory::create);

    EXPECT_TRUE(Registry::GetRequestHandler(Echo) != nullptr);
    EXPECT_TRUE(Registry::GetRequestHandler(Static) != nullptr);
    EXPECT_TRUE(Registry::GetRequestHandler(None) != nullptr);
}