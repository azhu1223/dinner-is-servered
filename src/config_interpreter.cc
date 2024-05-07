#include "config_interpreter.h"
#include "config_parser.h"
#include <boost/log/trivial.hpp>
#include <map>
#include <string>
#include <vector>
#include <stdexcept>


const int MINIMUM_VALID_PORT =0;
const int MAXIMUM_VALID_PORT = 65535;

//Retrieves the port from the Nginx config file
//Returns the port if found, otherwise returns 80 (default)
//If port is set to a non integer, will still retrun 80
int getPort(NginxConfig &config){
    int port = 80;

    for (const auto& statement : config.statements_) {
        // Find the server statement
        if (statement->tokens_[0] == "server") {
            for (const auto& nestedStatement : statement->child_block_->statements_) {
                // Find the listen statement
                if (nestedStatement->tokens_[0] == "listen") {
                    try{
                        port = std::stoi(nestedStatement->tokens_[1]);
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

    for (const auto& statement : config.statements_) {
        BOOST_LOG_TRIVIAL(info) << "Finding server paths";
        // Find the server statement
        if (statement->tokens_[0] == "server") {
            for (const auto& lv2Statement : statement->child_block_->statements_) {
                //Find the paths statement   
                if (lv2Statement->tokens_[0] == "paths") {
                    for (const auto& lv3statement : lv2Statement->child_block_->statements_) {
                        // Find all echo statements
                        if (lv3statement->tokens_[0] == "echo") {
                            BOOST_LOG_TRIVIAL(info) << "Adding echo path: " << lv3statement->tokens_[1];
                            echo_paths.push_back(lv3statement->tokens_[1]);
                        }
                        //Find all static statements
                        else if (lv3statement->tokens_[0] == "static") {
                            if (static_paths_to_server_paths.find(lv3statement->tokens_[1]) != static_paths_to_server_paths.end()){
                                BOOST_LOG_TRIVIAL(fatal) << "Duplicate static path: " << lv3statement->tokens_[1];
                                throw std::runtime_error("Duplicate static path");
                            }
                            BOOST_LOG_TRIVIAL(info) << "Adding static path: " << lv3statement->tokens_[1] << " mapping to server location " << lv3statement->tokens_[2];
                            static_paths_to_server_paths[lv3statement->tokens_[1]] = lv3statement->tokens_[2];
                        }
                    }
                }
            }
        }
    }

    //If no paths are set, by default add root to an echo path
    if (echo_paths.size() == 0 && static_paths_to_server_paths.size() == 0){
        BOOST_LOG_TRIVIAL(error) << "No paths found. Adding root as an echo path.";;
        echo_paths.push_back("/");
    }


    ServerPaths paths;
    paths.echo_ = echo_paths;
    paths.static_ = static_paths_to_server_paths;


    return paths;
}