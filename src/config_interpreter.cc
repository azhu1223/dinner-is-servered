#include "config_interpreter.h"
#include "config_parser.h"
#include <boost/log/trivial.hpp>
#include <map>
#include <string>
#include <vector>
#include <stdexcept>
#include <set>

// Initialize static members
ServerPaths ConfigInterpreter::server_paths_;
std::string ConfigInterpreter::api_key_;
std::string ConfigInterpreter::chatgpt_url_;

// Retrieves the port from the Nginx config file
// Returns the port if found, otherwise returns 80 (default)
// If port is set to a non integer, will still return 80
int ConfigInterpreter::getPort(NginxConfig &config){
    int port = 80;
    const int MINIMUM_VALID_PORT =0;
    const int MAXIMUM_VALID_PORT = 65535;

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

void ConfigInterpreter::setServerPaths(NginxConfig &config){
    std::vector<std::string> echo_paths;
    std::map<std::string, std::string> static_paths_to_server_paths;
    std::map<std::string, std::string> crud_paths_to_server_paths;
    std::set<std::string> used_locations;
    std::vector<std::string> health_paths;
    std::vector<std::string> sleep_paths;
    std::vector<std::string> app_paths;

    for (const auto& statement : config.statements_) {
        BOOST_LOG_TRIVIAL(info) << "Finding server paths";
        // Find the server statement
        if (statement->tokens_.at(0) == "server") {
            for (const auto& lv2Statement : statement->child_block_->statements_) {
                // Find all location statements   
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
                            // Find the server file location
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
                    //Crud statements
                    else if (location_type == "CrudHandler") {
                        bool found_server_file = false;
                        for (const auto& lv3statement : lv2Statement->child_block_->statements_) {
                            // Find the server file location
                            if (lv3statement->tokens_.at(0) == "data_path") {
                                if (found_server_file) {
                                    BOOST_LOG_TRIVIAL(fatal) << "Duplicate server file location";
                                    throw std::runtime_error("Duplicate server file location");
                                }
                                BOOST_LOG_TRIVIAL(info) << "Adding crud path: " << location << " mapping to server location " << lv3statement->tokens_.at(1);
                                crud_paths_to_server_paths[location] = lv3statement->tokens_.at(1);
                                found_server_file = true;
                            }
                        }
                    }
                    //Health Handler
                    else if (location_type == "HealthHandler") {
                        BOOST_LOG_TRIVIAL(info) << "Adding health path: " << location;
                        health_paths.push_back(location);
                    }
                    //Sleep Handler
                    else if (location_type == "SleepHandler") {
                        BOOST_LOG_TRIVIAL(info) << "Adding sleep path: " << location;
                        sleep_paths.push_back(location);
                    }
                    
                    //App Handler
                    // else if (location_type == "AppHandler") {
                    //     BOOST_LOG_TRIVIAL(info) << "Adding app path: " << location;
                    //     app_paths.push_back(location);
                    // }

                    else if (location_type == "AppHandler") {
                        bool found_gpt_url = false;
                        bool found_api_key = false;
                        for (const auto& lv3statement : lv2Statement->child_block_->statements_) {
                            // Find the server file location
                            if (lv3statement->tokens_.at(0) == "api_key") {
                                if (found_api_key) {
                                    BOOST_LOG_TRIVIAL(fatal) << "Duplicate API key";
                                    throw std::runtime_error("Duplicate API key");
                                }
                                api_key_ = lv3statement->tokens_.at(1);
                                found_api_key = true;
                            } 
                            else if (lv3statement->tokens_.at(0) == "chatgpt_url") {
                                if (found_gpt_url) {
                                    BOOST_LOG_TRIVIAL(fatal) << "Duplicate GPT url";
                                    throw std::runtime_error("Duplicate GPT url");
                                }
                                chatgpt_url_ = lv3statement->tokens_.at(1);
                                found_gpt_url = true;
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
    paths.crud_ = crud_paths_to_server_paths;
    paths.health_ = health_paths;
    paths.sleep_ = sleep_paths;
    paths.app_ = app_paths;
    server_paths_ = paths;
}

ServerPaths ConfigInterpreter::getServerPaths(){
    return server_paths_;
}

void ConfigInterpreter::setServerPaths(ServerPaths server_paths){
    server_paths_ = server_paths;
}

// void ConfigInterpreter::set_api_key(const std::string& api_key) {
//     api_key_ = api_key;
// }

std::string ConfigInterpreter::get_api_key() {
    return api_key_;
}

// void ConfigInterpreter::set_chatgpt_url(const std::string& chatgpt_url) {
//     chatgpt_url_ = chatgpt_url;
// }

std::string ConfigInterpreter::get_chatgpt_url() {
    return chatgpt_url_;
}
