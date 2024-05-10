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

# Specify the destination directory in the build directory
set(RESOURCES_DEST ${CMAKE_CURRENT_BINARY_DIR})

# Copy the resources folder to the build directory during the build process
file(COPY ${RESOURCES_DIR} DESTINATION ${RESOURCES_DEST})

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
add_library(request_parser_lib src/request_parser.cc)
add_library(not_found_handler_lib src/not_found_handler.cc)

target_link_libraries(logger_lib
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
                    Boost::system 
                    Boost::log_setup 
                    Boost::log
                    )

target_link_libraries(not_found_handler_lib
                    request_handler_lib
                    Boost::system 
                    Boost::log_setup 
                    Boost::log
                    )

target_link_libraries(echo_handler_lib
                    request_handler_lib
                    Boost::system 
                    Boost::log_setup 
                    Boost::log
                    )

target_link_libraries(request_handler_lib
                    config_interpreter_lib
                    Boost::system 
                    Boost::log_setup 
                    Boost::log
                    )

target_link_libraries(request_parser_lib
                    config_interpreter_lib
                    Boost::system 
                    Boost::log_setup 
                    Boost::log
                    )

target_link_libraries(session_lib 
                    config_interpreter_lib
                    request_handler_lib
                    echo_handler_lib
                    static_handler_lib
                    not_found_handler_lib
                    request_parser_lib
                    Boost::system 
                    Boost::log_setup 
                    Boost::log
                    )

target_link_libraries(server_lib
                    logger_lib
                    session_lib
                    config_interpreter_lib
                    Boost::system 
                    Boost::log_setup 
                    Boost::log
                    )

# Executables and target Libraries
add_executable(server src/_main.cc)
target_link_libraries(server 
                    server_lib
                    config_parser_lib
                    config_interpreter_lib
                    logger_lib
                    signal_handler_lib
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
target_link_libraries(echo_handler_test echo_handler_lib gtest_main)

add_executable(not_found_handler_test tests/not_found_handler_test.cc)
target_link_libraries(not_found_handler_test not_found_handler_lib gtest_main)

add_executable(session_test tests/session_test.cc)
target_link_libraries(session_test session_lib request_parser_lib request_handler_lib not_found_handler_lib echo_handler_lib static_handler_lib logger_lib Boost::log_setup Boost::log gtest_main)

add_executable(signal_handler_test tests/signal_handler_test.cc)
target_link_libraries(signal_handler_test signal_handler_lib gtest_main Boost::log_setup Boost::log)

add_executable(logger_test tests/logger_test.cc)
target_link_libraries(logger_test logger_lib gtest_main Boost::log_setup Boost::log)

add_executable(request_parser_test tests/request_parser_test.cc)
target_link_libraries(request_parser_test request_parser_lib config_interpreter_lib Boost::log_setup Boost::log gtest_main)

add_executable(static_handler_test tests/static_handler_test.cc)
target_link_libraries(static_handler_test
                      static_handler_lib
                      gtest_main
                      Boost::system
                      Boost::log_setup
                      Boost::log)

# Add Integration Tests
if (NOT CMAKE_BUILD_TYPE STREQUAL Coverage)
    add_test(NAME IntegrationTests.example_config COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/tests/integration_tests/integration_test_example_config.py ${CMAKE_CURRENT_BINARY_DIR}/bin/server WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests/integration_tests/)
    add_test(NAME IntegrationTests.empty_config COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/tests/integration_tests/integration_test_empty_config.py ${CMAKE_CURRENT_BINARY_DIR}/bin/server WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests/integration_tests/)
    add_test(NAME IntegrationTests.missing_closing_parens COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/tests/integration_tests/integration_test_missing_closing_parentheses.py ${CMAKE_CURRENT_BINARY_DIR}/bin/server WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests/integration_tests/)
    add_test(NAME IntegrationTests.empty_brackets COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/tests/integration_tests/integration_test_empty_brackets.py ${CMAKE_CURRENT_BINARY_DIR}/bin/server WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests/integration_tests/)
    add_test(NAME IntegrationTests.multiple_end_brackets COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/tests/integration_tests/integration_test_multiple_end_brackets.py ${CMAKE_CURRENT_BINARY_DIR}/bin/server WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests/integration_tests/)
    add_test(NAME IntegrationTests.request_valid_file COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/tests/integration_tests/integration_test_requested_file_success.py ${CMAKE_CURRENT_BINARY_DIR}/bin/server WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests/integration_tests/)
    add_test(NAME IntegrationTests.request_invalid_file COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/tests/integration_tests/integration_test_requested_file_fail.py ${CMAKE_CURRENT_BINARY_DIR}/bin/server WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests/integration_tests/)
endif()

#Discover tests
gtest_discover_tests(config_parser_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(config_interpreter_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(echo_handler_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(not_found_handler_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(session_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(signal_handler_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(logger_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(request_parser_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)

gtest_discover_tests(static_handler_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)




# Generate Coverage Report
include(cmake/CodeCoverageReportConfig.cmake)
generate_coverage_report(TARGETS server_lib signal_handler_lib logger_lib session_lib config_parser_lib config_interpreter_lib request_handler_lib request_parser_lib echo_handler_lib not_found_handler_lib static_handler_lib TESTS config_parser_test config_interpreter_test not_found_handler_test echo_handler_test session_test signal_handler_test logger_test request_parser_test static_handler_test)