cmake_minimum_required(VERSION 3.10.0)
project(dinner-is-servered)

# Make sure developers do not run cmake in the main project directory, to keep
# build artifacts from becoming clutter
if (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
message(FATAL_ERROR "In-source builds not allowed.
Please make a new directory (called a build directory) and run
CMake from there.You may need to remove CMakeCache.txt." )
endif()

# Turn on debug builds if we are building in a devel shell
if (CMAKE_BUILD_TYPE STREQUAL "" AND "$ENV{DEVEL_SHELL}" STREQUAL "1")
message(STATUS "Setting debug build type by default in devel shell")
set(CMAKE_BUILD_TYPE Debug)
endif()

# Check if the build type is set for coverage
if(CMAKE_BUILD_TYPE STREQUAL "Coverage")
    message(STATUS "Configuring build for coverage")
    # Add flags for GCC to generate coverage data
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fprofile-arcs -ftest-coverage")
endif()


# Output binaries to a sub directory "bin"
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)


# Specify the source directory of your resources folder
set(RESOURCES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/resources)
set(APP_PAGES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/app_pages)


# Specify the destination directory in the build directory
set(RESOURCES_DEST ${CMAKE_CURRENT_BINARY_DIR})
set(APP_PAGES_DEST ${CMAKE_CURRENT_BINARY_DIR})


# Copy the resources folder to the build directory during the build process
file(COPY ${RESOURCES_DIR} DESTINATION ${RESOURCES_DEST})
file(COPY ${APP_PAGES_DIR} DESTINATION ${APP_PAGES_DEST})

#Enable libcurl
# Use static libraries so binaries can be deployed without a full curl install
set(CURL_USE_STATIC_LIBS TRUE)
find_package(CURL)
message(STATUS "Curl version: ${CURL_VERSION_STRING}")
message(STATUS "The CURL libraries are: ${CURL_LIBRARIES}")

# Find nlohmann_json package for json parsing GPT API responses and requests
find_package(nlohmann_json 3.2.0 REQUIRED)

# Enable cmake testing
include(CTest)
enable_testing()

# Enable GoogleTest
include(GoogleTest)
add_subdirectory(/usr/src/googletest googletest)

# Enable Boost
# Use static libraries so binaries can be deployed without a full boost install
set(Boost_USE_STATIC_LIBS ON)
find_package(Boost 1.50 REQUIRED COMPONENTS system log log_setup)
message(STATUS "Boost version: ${Boost_VERSION}")
message(STATUS "The Boost libraries are: ${Boost_LIBRARIES}")


include_directories(include)

# Libraries
add_library(server_lib src/server.cc)
add_library(session_lib src/session.cc)
add_library(config_parser_lib src/config_parser.cc)
add_library(config_interpreter_lib src/config_interpreter.cc)
add_library(request_handler_lib src/request_handler.cc)
add_library(logger_lib src/logger.cc)
add_library(signal_handler_lib src/signal_handler.cc)
add_library(echo_handler_lib src/echo_handler.cc)
add_library(static_handler_lib src/static_handler.cc)
add_library(crud_handler_lib src/crud_handler.cc)
add_library(crud_file_manager_lib src/crud_file_manager.cc)
add_library(request_dispatcher_lib src/request_dispatcher.cc)
add_library(not_found_handler_lib src/not_found_handler.cc)
add_library(registry_lib src/registry.cc)
add_library(health_handler_lib src/health_handler.cc)
add_library(sleep_handler_lib src/sleep_handler.cc)
add_library(logging_buffer_lib src/logging_buffer.cc)
add_library(app_handler_lib src/app_handler.cc)


target_link_libraries(logger_lib
                    Boost::system 
                    Boost::log_setup 
                    Boost::log
                    )

target_link_libraries(logging_buffer_lib
                    logger_lib
                    Boost::system 
                    Boost::log_setup 
                    Boost::log
                    )

target_link_libraries(signal_handler_lib
                    Boost::system 
                    Boost::log_setup 
                    Boost::log
                    )

target_link_libraries(server_lib
                    session_lib
                    logger_lib
                    Boost::system 
                    Boost::log_setup 
                    Boost::log
                    )

target_link_libraries(config_interpreter_lib
                    config_parser_lib
                    Boost::system 
                    Boost::log_setup 
                    Boost::log
                    )

target_link_libraries(static_handler_lib
                    request_handler_lib
                    request_dispatcher_lib
                    logging_buffer_lib
                    Boost::system 
                    Boost::log_setup 
                    Boost::log
                    )

target_link_libraries(crud_handler_lib
                    request_handler_lib
                    request_dispatcher_lib
                    crud_file_manager_lib
                    logging_buffer_lib
                    Boost::system 
                    Boost::log_setup 
                    Boost::log
                    )

target_link_libraries(crud_file_manager_lib
                    logging_buffer_lib
                    Boost::system 
                    Boost::log_setup 
                    Boost::log
                    )

target_link_libraries(not_found_handler_lib
                    request_handler_lib
                    logging_buffer_lib
                    Boost::system 
                    Boost::log_setup 
                    Boost::log
                    )

target_link_libraries(echo_handler_lib
                    request_handler_lib
                    logging_buffer_lib
                    Boost::system 
                    Boost::log_setup 
                    Boost::log
                    )

target_link_libraries(health_handler_lib
                    request_handler_lib
                    logging_buffer_lib
                    Boost::system 
                    Boost::log_setup 
                    Boost::log
                    )

target_link_libraries(app_handler_lib
                    request_handler_lib
                    logging_buffer_lib
                    Boost::system 
                    Boost::log_setup 
                    Boost::log
                    ${CURL_LIBRARIES}
                    nlohmann_json::nlohmann_json
                    )

target_link_libraries(sleep_handler_lib
                    request_handler_lib
                    logging_buffer_lib
                    Boost::system 
                    Boost::log_setup 
                    Boost::log
                    )

target_link_libraries(request_handler_lib
                    config_interpreter_lib
                    logging_buffer_lib
                    Boost::system 
                    Boost::log_setup 
                    Boost::log
                    )

target_link_libraries(request_dispatcher_lib
                    config_interpreter_lib
                    registry_lib
                    echo_handler_lib
                    static_handler_lib
                    health_handler_lib
                    sleep_handler_lib
                    app_handler_lib
                    crud_handler_lib
                    not_found_handler_lib
                    logging_buffer_lib 
                    Boost::system 
                    Boost::log_setup 
                    Boost::log
                    )

target_link_libraries(session_lib 
                    config_interpreter_lib
                    request_handler_lib
                    request_dispatcher_lib
                    logging_buffer_lib 
                    Boost::system 
                    Boost::log_setup 
                    Boost::log
                    )

target_link_libraries(server_lib
                    logger_lib
                    session_lib
                    config_interpreter_lib
                    logging_buffer_lib
                    Boost::system 
                    Boost::log_setup 
                    Boost::log
                    )

target_link_libraries(registry_lib
                    request_handler_lib
                    request_dispatcher_lib
                    )


# Executables and target Libraries
add_executable(server src/_main.cc)
target_link_libraries(server 
                    server_lib
                    config_parser_lib
                    config_interpreter_lib
                    logger_lib
                    signal_handler_lib
                    registry_lib
                    Boost::system 
                    Boost::log_setup 
                    Boost::log
                    )

#Test Executables and Target Libraries

add_executable(config_interpreter_test tests/config_interpreter_test.cc)
target_link_libraries(config_interpreter_test config_parser_lib config_interpreter_lib Boost::log_setup Boost::log gtest_main)

add_executable(config_parser_test tests/config_parser_test.cc)
target_link_libraries(config_parser_test config_parser_lib config_interpreter_lib logger_lib Boost::log_setup Boost::log gtest_main)

add_executable(echo_handler_test tests/echo_handler_test.cc)
target_link_libraries(echo_handler_test echo_handler_lib logging_buffer_lib gtest_main)

add_executable(server_test tests/server_test.cc)
target_link_libraries(server_test server_lib gtest_main)

add_executable(not_found_handler_test tests/not_found_handler_test.cc)
target_link_libraries(not_found_handler_test not_found_handler_lib logging_buffer_lib gtest_main)

add_executable(health_handler_test tests/health_handler_test.cc)
target_link_libraries(health_handler_test health_handler_lib logging_buffer_lib gtest_main)

add_executable(sleep_handler_test tests/sleep_handler_test.cc)
target_link_libraries(sleep_handler_test sleep_handler_lib logging_buffer_lib  Boost::log_setup Boost::log gtest_main)

add_executable(app_handler_test tests/app_handler_test.cc)
target_link_libraries(app_handler_test app_handler_lib logging_buffer_lib gtest_main Boost::log_setup Boost::log)


add_executable(session_test tests/session_test.cc)
target_link_libraries(session_test session_lib logging_buffer_lib request_dispatcher_lib request_handler_lib not_found_handler_lib echo_handler_lib static_handler_lib logger_lib Boost::log_setup Boost::log gtest_main)

add_executable(signal_handler_test tests/signal_handler_test.cc)
target_link_libraries(signal_handler_test signal_handler_lib gtest_main Boost::log_setup Boost::log)

add_executable(logger_test tests/logger_test.cc)
target_link_libraries(logger_test logger_lib gtest_main Boost::log_setup Boost::log)

add_executable(request_dispatcher_test tests/request_dispatcher_test.cc)
target_link_libraries(request_dispatcher_test request_dispatcher_lib logging_buffer_lib config_interpreter_lib Boost::log_setup Boost::log gtest_main)

add_executable(static_handler_test tests/static_handler_test.cc)
target_link_libraries(static_handler_test
                      static_handler_lib
                      logging_buffer_lib
                      gtest_main
                      Boost::system
                      Boost::log_setup
                      Boost::log)

add_executable(crud_handler_test tests/crud_handler_test.cc)
target_link_libraries(crud_handler_test
                      crud_handler_lib
                      logging_buffer_lib
                      gtest_main
                      Boost::system
                      Boost::log_setup
                      Boost::log)

add_executable(crud_file_manager_test tests/crud_file_manager_test.cc)
target_link_libraries(crud_file_manager_test
                      crud_file_manager_lib
                      crud_handler_lib
                      logging_buffer_lib
                      gtest_main
                      Boost::system
                      Boost::log_setup
                      Boost::log)

add_executable(registry_test tests/registry_test.cc)
target_link_libraries(registry_test registry_lib logging_buffer_lib echo_handler_lib static_handler_lib not_found_handler_lib gtest_main)

add_executable(logging_buffer_test tests/logging_buffer_test.cc)
target_link_libraries(logging_buffer_test logging_buffer_lib gtest_main)


# Add Integration Tests
if (NOT CMAKE_BUILD_TYPE STREQUAL Coverage)
    add_test(NAME IntegrationTests.example_config COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/tests/integration_tests/integration_test_example_config.sh ${CMAKE_CURRENT_BINARY_DIR}/bin/server WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests/integration_tests/)
    add_test(NAME IntegrationTests.empty_config COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/tests/integration_tests/integration_test_empty_config.sh ${CMAKE_CURRENT_BINARY_DIR}/bin/server WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests/integration_tests/)
    add_test(NAME IntegrationTests.missing_closing_parens COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/tests/integration_tests/integration_test_missing_closing_parentheses.sh ${CMAKE_CURRENT_BINARY_DIR}/bin/server WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests/integration_tests/)
    add_test(NAME IntegrationTests.empty_brackets COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/tests/integration_tests/integration_test_empty_brackets.sh ${CMAKE_CURRENT_BINARY_DIR}/bin/server WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests/integration_tests/)
    add_test(NAME IntegrationTests.multiple_end_brackets COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/tests/integration_tests/integration_test_multiple_end_brackets.sh ${CMAKE_CURRENT_BINARY_DIR}/bin/server WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests/integration_tests/)
    add_test(NAME IntegrationTests.request_valid_file COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/tests/integration_tests/integration_test_requested_file_success.sh ${CMAKE_CURRENT_BINARY_DIR}/bin/server WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests/integration_tests/)
    add_test(NAME IntegrationTests.request_invalid_file COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/tests/integration_tests/integration_test_requested_file_fail.sh ${CMAKE_CURRENT_BINARY_DIR}/bin/server WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests/integration_tests/)
    add_test(NAME IntegrationTests.crud_post COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/tests/integration_tests/integration_test_crud_post.sh ${CMAKE_CURRENT_BINARY_DIR}/bin/server WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests/integration_tests/)
    add_test(NAME IntegrationTests.crud_get COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/tests/integration_tests/integration_test_crud_get.sh ${CMAKE_CURRENT_BINARY_DIR}/bin/server WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests/integration_tests/)
    add_test(NAME IntegrationTests.crud_put COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/tests/integration_tests/integration_test_crud_put.sh ${CMAKE_CURRENT_BINARY_DIR}/bin/server WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests/integration_tests/)
    add_test(NAME IntegrationTests.crud_delete COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/tests/integration_tests/integration_test_crud_delete.sh ${CMAKE_CURRENT_BINARY_DIR}/bin/server WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests/integration_tests/)
    add_test(NAME IntegrationTests.crud_list COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/tests/integration_tests/integration_test_crud_list.sh ${CMAKE_CURRENT_BINARY_DIR}/bin/server WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests/integration_tests/)
    add_test(NAME IntegrationTests.health COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/tests/integration_tests/integration_test_health.sh ${CMAKE_CURRENT_BINARY_DIR}/bin/server WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests/integration_tests/)
    add_test(NAME IntegrationTests.multithread COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/tests/integration_tests/integration_test_multithread.sh ${CMAKE_CURRENT_BINARY_DIR}/bin/server WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests/integration_tests/)
endif()

#Discover tests
gtest_discover_tests(config_parser_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(config_interpreter_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(echo_handler_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(not_found_handler_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(session_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(signal_handler_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(logger_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(request_dispatcher_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(health_handler_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(static_handler_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(crud_handler_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(crud_file_manager_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(registry_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(server_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(logging_buffer_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(sleep_handler_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(app_handler_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)


# Generate Coverage Report
include(cmake/CodeCoverageReportConfig.cmake)
generate_coverage_report(TARGETS 
                            server_lib 
                            signal_handler_lib 
                            logger_lib 
                            session_lib 
                            config_parser_lib 
                            config_interpreter_lib 
                            request_handler_lib 
                            request_dispatcher_lib 
                            echo_handler_lib 
                            health_handler_lib 
                            not_found_handler_lib 
                            static_handler_lib 
                            crud_handler_lib 
                            crud_file_manager_lib
                            registry_lib 
                            logging_buffer_lib
                            app_handler_lib
                        TESTS 
                            config_parser_test
                            config_interpreter_test
                            not_found_handler_test 
                            echo_handler_test 
                            session_test 
                            server_test 
                            signal_handler_test 
                            logger_test 
                            health_handler_test
                            sleep_handler_test
                            request_dispatcher_test 
                            static_handler_test 
                            crud_handler_test 
                            crud_file_manager_test 
                            registry_test
                            logging_buffer_test
                            app_handler_test
                            )