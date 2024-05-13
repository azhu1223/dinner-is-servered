//An interpreter to derive values from an Nginx config file

#ifndef CONFIG_INTERPRETER_H
#define CONFIG_INTERPRETER_H

#include "config_parser.h"
#include <vector>
#include <map>
#include  <string>
#include "utils.h"

class ConfigInterpreter{
    public:
        //Retrieves the port from the Nginx config file
        //Returns the port if found, otherwise returns 80 (default)
        //If port is set to a non integer, will still retrun 80
        static int getPort(NginxConfig &config);

        static void setServerPaths(NginxConfig &config);

        static void setServerPaths(ServerPaths server_paths);


        static ServerPaths getServerPaths();
    
    private:
        static ServerPaths server_paths_;

};

#endif