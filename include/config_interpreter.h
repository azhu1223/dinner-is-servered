//An interpreter to derive values from an Nginx config file

#ifndef CONFIG_INTERPRETER_H
#define CONFIG_INTERPRETER_H

#include "config_parser.h"
#include <vector>
#include <string>

//Struct for paths
struct ServerPaths{
    std::vector<std::string> echo_;
    std::vector<std::string> static_;
};

//Retrieves the port from the Nginx config file
//Returns the port if found, otherwise returns 80 (default)
//If port is set to a non integer, will still retrun 80
int getPort(NginxConfig &config);


//Retrieves the paths from the Nginx config file
//Returns: [ [ECHO_PATHS], [STATIC_PATHS]]
ServerPaths getServerPaths(NginxConfig &config);

#endif