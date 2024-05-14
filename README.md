# dinner-is-servered

## Overview

dinner-is-served is a web server that supports echoing requests recieved and staic file handling. **It is reccomended to use the CS130 development environment.** 

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

During the build process, all resources from the *resources* directory will be copied to the build direcotry, so the server may access them.

## Testing

The *build-server.sh* shell script will run tests automatically. Alternatively, you can manually ruin tests from the build directory with the command:

`make test`

The test results are viewable in the terminal. To view the test logs, you can examine the contentes of the *Testing* directory located in the build directory.

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

This will build the server, run all tests, and provide a test coverage report. You can view the basic details of the coverage report in the console. To view the results in greater depth, you can view the report, which is in the *build_coverage/report* directory. Open the *index.html* file to view the results.

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

All configuration files must meet the specifrications detailed here: https://www.cs130.org/assignments/6/#config-file-format

Configuration files for the server are stored in the *confs* directory. The *sample_conf* file is an example of a configuration file, and is the one currently used with Docker. To change the configuration file used with Docker, change the file *docker/Dockerfile* to specify to copy the new configuration file and to pass it as an argument when running the server.

## Repository Guide

- All header files for the project are located within the *include* directory. It is crucial to place header files in this directory, so they may be accessed by source files during the build process.

- All resources that are to be made available to the server are located within the *resources* directory. It is crucial to place the resources to serve in this folder, so that they are copied over to the build.

- All implementation files are located in the *src* directory.

- All test files are located in the *test* directory. Unit tests are located directly in the *test* directory, and integration tests are located in the *test/integration_tests* directory. There are additional subdirectories for *configs, resources,* and *integration_test_expected_outputs* in the *test* directory.

- All configuration files are located in the *confs* directory. See the configuration section above for more details pertaining to copying them for Docker.

- The CMakeLists.txt file is located in the repository's root directory. The *cmake* folder contains all functions utilized by the CMakeLists.txt file.

- All files pretaining to the Docker containerization are located in the *docker* directory.

## File Serving

File serving is available on the static paths specified in the configuration file upon which the server is running. It is **highly reccomended** to place all resources to serve in the *resources* directory, so that they will be properly copied to the build directory. Moreover, since the server uses relative paths to specify static file paths, each *root* listed in the configutarion file should begin with `../resources/` in order to properly direct the server to the resources directory. From there you can append on to specificy a more explicit path within the resources directory.

## Logging

All logging is done through Boost Log (https://www.boost.org/doc/libs/1_74_0/libs/log/doc/html/index.html). Log messages are output to the console as well as to logging files, which are rotated as specificed here (https://www.cs130.org/assignments/4/#add-logging-to-your-web-server).

File logging can be disabled when running the server via the `--disable-file-logging` flag.

## Current Source Code Overview

**TODO**


## Development

**TODO**

## Test Development

Adding to cmake **TODO**

### Integration Tests

In order to minimize the output of log files when running tests, it is reccomended that integration tests run the server with the `--disable-file-logging` flag. All logs from the server will still be available in the *LastTest.log* file located within the *Testing* directory. The command to run the server for integration tests should be as such:

`server CONFIGURATION_FILE --disable-file-logging`

### Unit Tests

**TODO**

## Google Cloud

Google cloud runs a VM instance containing the Docker conatiner specified in *docker/cloudbuild.yaml*. Please see the various files in the *docker* directory to gain a deeper understading of the Docker build process.

Upon merging a branch to the main branch of the dinner-is-servered remote Gerrit repository (https://code.cs130.org/q/project:dinner-is-servered+branch:main), a build is started on Google Cloud. After merging a change, it is crucial to verify that the build succeeds and if not, to revert the change immediately. 

After a build has succeeded, it can be deployed by restatring the *web-server* web instance. Once the server has restarted, it is reccommended to issue requests to the server's External IP (34.82.128.18) to verify that the server is running. 