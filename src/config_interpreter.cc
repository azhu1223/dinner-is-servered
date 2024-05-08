#include "config_interpreter.h"
#include "config_parser.h"
#include <boost/log/trivial.hpp>
#include <map>
#include <string>
#include <vector>
#include <stdexcept>
#include <set>


const int MINIMUM_VALID_PORT =0;
const int MAXIMUM_VALID_PORT = 65535;

//Retrieves the port from the Nginx config file
//Returns the port if found, otherwise returns 80 (default)
//If port is set to a non integer, will still retrun 80
int getPort(NginxConfig &config){
    int port = 80;

    for (const auto& statement : config.statements_) {
        // Find the server statement
        if (statement->tokens_.at(0) == "server") {
            for (const auto& nestedStatement : statement->child_block_->statements_) {
                // Find the listen statement
                if (nestedStatement->tokens_.at(0) == "listen") {
                    try{
                        port = std::stoi(nestedStatement->tokens_.at(1));
                    }catch(const std::invalid_argument& ia){
                        BOOST_LOG_TRIVIAL(error) << "Invalid argument for port. Expected an integer. Setting port to 80.";
                        port =80;
                    }catch(const std::out_of_range& oor){
                        BOOST_LOG_TRIVIAL(error) << "Port out of range. Setting port to 80.";
                        port =80;
                    }
                    //Verify port is within valid range
                    if (port < MINIMUM_VALID_PORT || port > MAXIMUM_VALID_PORT){
                        BOOST_LOG_TRIVIAL(error) << "Port out of range. Setting port to 80.";
                        port =80; 
                    }

                }
            }
        }
    }
    return port;
}

//Retrieves the paths from the Nginx config file
//Returns: [ [ECHO_PATHS], <STATIC_PATHS, SERVER_LOCATION> ]
ServerPaths getServerPaths(NginxConfig &config){
    std::vector<std::string> echo_paths;
    std::map<std::string, std::string> static_paths_to_server_paths;
    std::set<std::string> used_locations;


    for (const auto& statement : config.statements_) {
        BOOST_LOG_TRIVIAL(info) << "Finding server paths";
        // Find the server statement
        if (statement->tokens_.at(0) == "server") {
            for (const auto& lv2Statement : statement->child_block_->statements_) {
                //Find all location statements   
                BOOST_LOG_TRIVIAL(info) << lv2Statement->tokens_.at(0);
                if (lv2Statement->tokens_.at(0) == "location") {
                    std::string location = lv2Statement->tokens_.at(1);
                    std::string location_type = lv2Statement->tokens_.at(2);

                    if (used_locations.find(location) != used_locations.end()){
                        BOOST_LOG_TRIVIAL(fatal) << "The same location (" << location <<") is specified for multiple handlers";
                        throw std::runtime_error("The same location (" + location + ") is specified for multiple handlers");
                    }
                    used_locations.insert(location);

                    //Echo statements
                    if (location_type == "EchoHandler") {
                        BOOST_LOG_TRIVIAL(info) << "Adding echo path: " << location;
                        echo_paths.push_back(location);
                    }
                    //Static statements
                    else if (location_type == "StaticHandler") {
                        bool found_server_file = false;
                        for (const auto& lv3statement : lv2Statement->child_block_->statements_) {
                            //Find the server file location
                            if (lv3statement->tokens_.at(0) == "root") {
                                if (found_server_file) {
                                    BOOST_LOG_TRIVIAL(fatal) << "Duplicate server file location";
                                    throw std::runtime_error("Duplicate server file location");
                                }
                                BOOST_LOG_TRIVIAL(info) << "Adding static path: " << location << " mapping to server location " << lv3statement->tokens_.at(1);
                                static_paths_to_server_paths[location] = lv3statement->tokens_.at(1);
                                found_server_file = true;
                            }
                        }
                    }
                }
            }
        }
    }

    ServerPaths paths;
    paths.echo_ = echo_paths;
    paths.static_ = static_paths_to_server_paths;


    return paths;
}