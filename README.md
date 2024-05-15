# dinner-is-servered

## Overview

dinner-is-served is a web server that supports echoing requests recieved and static file handling. **It is reccomended to use the CS130 development environment.** 

## Build

Upon cloning the repository, create a build directory. (Note: the build directory is ignored by git)

`mkdir build`

The simplest way to build the server is to call the *build-server.sh* shell script from the base directory of the repository:

`./build-server.sh`

Alternatively, you can run the following build commands manually:

```
cd build
cmake ..
make
```

During the build process, all resources from the *resources* directory will be copied to the build directory, so the server may access them.

## Testing

The *build-server.sh* shell script will run tests automatically. Alternatively, you can manually run tests from the build directory with the command:

`make test`

The test results are viewable in the terminal. To view the test logs, you can examine the contents of the *Testing/Temporary* directory located in the build directory.

### Test Coverage

Upon cloning the repository, create a build_coverage directory. (Note: the build_coverage directory is ignored by git)

`mkdir build_coverage`

The simplest way to build the server and run the coverage report is to call the *build-coverage.sh* shell script from the base directory of the repository:

`./build-coverage.sh`

Alternatively, you can run the following build commands manually:

```
cd build_coverage
cmake -DCMAKE_BUILD_TYPE=Coverage .. 
make coverage
```

This will build the server, run all unit tests, and provide a test coverage report. You can view the basic details of the coverage report in the console. To view the results in greater depth, you can view the report, which is in the *build_coverage/report* directory. Open the *index.html* file to view the results.

## Running

Execute the *server* binary from the build directory, passing in an Nginx configuration file as its only parameter. To run with our sample configuration file (which is the file that is used with Google Cloud) run the following command:

```
cd build
./bin/server ../confs/sample_conf
```

This will run the server locally. Logging messages should appear in the terminal indicating the server is starting up.

In a new terminal from the same environment, you can issue requests to server. For example:

`curl -i localhost`


## Docker

To run a Docker container with the server running in it, you can run the *docker/init.sh* shell script from the repository's root directory:

`./docker/init.sh`

Alternatively, you can run the commands:
```
docker build -f docker/base.Dockerfile -t dinner-is-servered:base .
docker build -f docker/Dockerfile -t dinner-is-servered .
docker run --rm -dp 127.0.0.1:80:80 --name my_run dinner-is-servered:latest
```
This process first builds a base with the necessary OS and binaries, then the server's Docker container, and finally runs the server's Docker container.

In a new terminal from your machine (not from the development environment), you can issue requests to server. For example:

`curl -i localhost`

## Configuration

All configuration files must meet the specifications detailed [here](https://www.cs130.org/assignments/6/#config-file-format).

Configuration files for the server are stored in the *confs* directory. The *sample_conf* file is an example of a configuration file, and is the one currently used with Docker. To change the configuration file used with Docker, change the file *docker/Dockerfile* to specify to copy the new configuration file and to pass it as an argument when running the server.

## Repository Guide

- All header files for the project are located within the *include* directory. Header files must be placed in this directory, so they may be accessed by source files during the build process.

- All resources that are to be made available to the server are located within the *resources* directory. Resources to served must be placed in this folder, so that they are copied over to the build.

- All implementation files are located in the *src* directory.

- All test files are located in the *test* directory. Unit tests are located directly in the *test* directory, and integration tests are located in the *test/integration_tests* directory. There are additional subdirectories for *configs, resources,* and *integration_test_expected_outputs* in the *test* directory.

- All configuration files are located in the *confs* directory. See the configuration section above for more details pertaining to copying them for Docker.

- The CMakeLists.txt file is located in the repository's root directory. The *cmake* folder contains all functions utilized by the CMakeLists.txt file.

- All files pretaining to the Docker containerization are located in the *docker* directory.

## File Serving

File serving is available on the static paths specified in the configuration file supplied to the server upon starting. It is **highly recommended** to place all resources to serve in the *resources* directory, so that they will be properly copied to the build directory. Moreover, since the server uses relative paths to specify static file paths, each *root* listed in the configuration file should begin with `../resources/` in order to properly direct the server to the resources directory. From there you can append on to specify a more explicit path within the resources directory.

## Logging

All logging is done through [Boost Log](https://www.boost.org/doc/libs/1_74_0/libs/log/doc/html/index.html). Log messages are outputed to the console as well as to logging files, which are rotated as specificed [here](https://www.cs130.org/assignments/4/#add-logging-to-your-web-server).

File logging can be disabled when running the server via the `--disable-file-logging` flag.

## Current Source Code Overview

**TODO**


## Development

To add a new request handler, it must inherit from and implement the interface defined by RequestHander in RequestHandler.h. The new handler must implement the pure virtual function *handle_request*. This function receives an *http::request\<http::vector_body<char>\>* and returns a *http::response\<http::vector_body<char>\>*, where *http* is the namespace *boost::beast::http*. 

Next, A factory class must be defined that contains a *create* function that has no arguments and only returns a dynamically allocated instance of the new request handler. 

A new enum that represents the new request type handled by this handler must be added to the enum RequestType in *request_dispatcher.h*. Then, a line must be added to _main.cc to register the new factory function with the Registry and associate the new enum that was created with its factory function. This should be done with the function *Registry::RegisterRequestHandler*, which takes in the enum defined previously, as well as the factory function defined above.

Finally, *RequestDispatcher::getRequestType* must be modified to be able to detect the type of request a response target represents.

An example of the above process is given with NotFoundHandler:
The class and its factory class is declared in *not_found_handler.h* and their functions are defined in *not_found_handler.cc*. Its *RequestType* enum defined in *request_dispatcher.h* is called *None*. It's registered to the Registry in *_main.cc* with the line
```
Registry::RegisterRequestHandler(None, NotFoundHandlerFactory::create);
```
Finally, any request with a target that doesn't match any path provided in the config file provided to the server results in *None* being returned from *RequestDispatcher::getRequestType*, signifying that the NotFoundHandler should be dispatched. In addition, targets that match with a path designated for static file serving, but which also reference files that do not actually exist also result in *None* being returned.
## Test Development

To set up additional unit tests, depending on the class(es) you would like to test, you will need to add the implementation of the tests in a test file under /tests directory. Most tests we have written use either basic isolated tests (TEST() in GTest), or fixtures (TEST_F()) to share variables and other state elements. Other test files in this directory provide a good reference guide. Then, to ensure that these tests are run, you will also need to amend the CMakeLists.txt to integrate into execution. Start by adding an executable with an alias for the file, as shown in an example below (sets alias `server_test`). Additionally, you need to link the test file with its corresponding alias library and with gtest_main, also as shown below - 

add_executable(server_test tests/server_test.cc)
target_link_libraries(server_test server_lib gtest_main)

Then, add a command for gtest to run the tests, also setting a working directory as "reference" for the test file, as shown here - 
gtest_discover_tests(server_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)

Finally, to include the tests in the coverage report, you will need to add the test alias under the TESTS section of generate_coverage_report (at bottom of CMakeLists), shown here. 

generate_coverage_report(TARGETS server_lib signal_handler_lib logger_lib session_lib config_parser_lib config_interpreter_lib request_handler_lib request_dispatcher_lib echo_handler_lib not_found_handler_lib static_handler_lib registry_lib TESTS config_parser_test config_interpreter_test not_found_handler_test echo_handler_test session_test server_test signal_handler_test logger_test request_dispatcher_test static_handler_test registry_test)

Note that this setup is only needed if you intend to create new test files. If you intend to operate in an existing test file, you just need to remake the build/build-coverage directory after writing the tests and should be good to go. If you are adding tests in such a way that the file dependencies are changed, then you will also need to make an adjustment to the target_link_libraries() portion of the corresponding alias in CMakeLists. For example, currently, these are the dependencies for signal_handler, as outlined here - 

target_link_libraries(signal_handler_lib
                    Boost::system 
                    Boost::log_setup 
                    Boost::log
                    )

If, for some reason, you want to write tests such that signal_handler_lib is dependent on another file (say logger_lib), you will need to append logger_lib executable into this function, which will make it look like this - 

target_link_libraries(signal_handler_lib
                    Boost::system 
                    Boost::log_setup 
                    Boost::log
                    logger_lib
                    )

To set up additional integration tests, you will need to add test scripts under test/integration_tests (see prior implementations on how to create such tests), with examples of expected outputs in tests/integration_test_expected_outputs. To add these tests to CMakeLists, simply add it as such in the section declaring integration tests. You will need to change the extension of the IntegrationTests string to your test name, and change the current file for integration test under the COMMAND flag. Everything else can be left the same. See example below for elucidation.  

    add_test(NAME IntegrationTests.example_config COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/tests/integration_tests/integration_test_example_config.py ${CMAKE_CURRENT_BINARY_DIR}/bin/server WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests/integration_tests/)


### Integration Tests

In order to minimize the output of log files when running tests, it is recommended that integration tests run the server with the `--disable-file-logging` flag. All logs from the server will still be available in the *LastTest.log* file located within the *Testing* directory. The command to start the server within integration tests should be:

`server CONFIGURATION_FILE --disable-file-logging`

### Unit Tests

To run the unit tests, see **make test** above. This repository has approximately 71 unit tests for a variety of implementations, the overview of which is provided below - 

(1) config_interpreter_test - 
This code tests the behavior of the ConfigInterpreter class for handling server configuration files, including port parsing and server path extraction, ensuring correct defaults, error handling, and path validation.

(2) config_parser_test - Tests the behavior NginxConfigParser class for parsing various Nginx configuration file scenarios, ensuring correct handling of configurations, comments, blocks, whitespace, nested structures, and error conditions.

(3) echo_handler_test - 
Tests the EchoHandler class, verifying its request handling functionality and the creation of an instance via the EchoHandlerFactory. The tests ensure that given the appropriate echo type request, the response, and it's properties (including status code, content type, content length, body) are correct. 

(4) logger_test - 
Barebones test for the initialize_logging function for Boost Logging, ensuring that it does not throw any exceptions during initialization when called with either true or false as an argument. Since this is an external library's functionality that this implementation borrows from, there wasn't much to test on. We basically assume it works. 

(5) not_found_handler_test - 
Tests the NotFoundHandler class, verifying that when invoked in case of a 404, it sends the appropriate response (in terms of content, status, and length). It also tests the NotFoundHandlerFactory to ensure it properly creates instances of NotFoundHandler.

(6) registry_test - Tests Registry class, verifying the retrieval of different RequestHandler types that have been registered, including EchoHandler, StaticHandler, and NotFoundHandler, and checking the behavior for a particular handler type that hasn't been registered (dummy RequestType object), which in this case we called BogusType.

(7) request_dispatcher_test - Tests RequestDispatcher, verifying the correct identification of request types and the retrieval of static file paths based on the given request targets. It also tests that when given a static/echo/404 request, the RequestDispatcher identifies the appropriate type of the request. 

(8) server_test - Tests the constructor and start_accept method of the server class, ensuring that it returns true upon successful initiation of accepting connections. 

(9) session_test - This code tests the session class, verifying all aspects of socket connection and management are properly controlled.It checks that session initializes correctly, manages its socket and communication operations properly, handles errors, and closes cleanly.

(10) signal_handler_test - This code tests the sig_handler function from signal_handler, verifying that it correctly handles a SIGINT signal by setting up the signal handler and ensuring that the program terminates as expected when the signal is raised; not too much testing here since this functionality is essentially derived from an external library. 

(11) static_handler_test - This code tests the StaticHandler class (for static type requests), ensuring correct content type determination, request handling for both existing and non-existent files, and creation via the factory. The tests cover scenarios for valid file requests, unsupported file types, and handler creation.

## Google Cloud

Google cloud runs a VM instance containing the Docker container specified in *docker/cloudbuild.yaml*. Please see the various files in the *docker* directory to gain a deeper understanding of the Docker build process.

Upon merging a branch to the main branch of the dinner-is-servered remote [Gerrit repository](https://code.cs130.org/q/project:dinner-is-servered+branch:main), a build is started on Google Cloud. After merging a change, it is crucial to verify that the build succeeds and if not, to revert the change immediately. 

After a build has succeeded, it can be deployed by restarting the *web-server* web instance. Once the server has restarted, it is reccommended to issue requests to the server's External IP (34.82.128.18) to verify that the server is running. 