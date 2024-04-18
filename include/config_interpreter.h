//An interpreter to derive values from an Nginx config file

#ifndef CONFIG_INTERPRETER_H
#define CONFIG_INTERPRETER_H

#include "config_parser.h"

//Retrieves the port from the Nginx config file
//Returns the port if found, otherwise returns 80 (default)
//If port is set to a non integer, will still retrun 80
int getPort(NginxConfig &config);

#endif