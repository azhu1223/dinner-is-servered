
#include "config_interpreter.h"
#include "config_parser.h"

const int MINIMUM_VALID_PORT =0;
const int MAXIMUM_VALID_PORT = 65536;

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
                        std::cerr << "Invalid argument for port. Expected an integer" << std::endl;
                        port =80;
                    }catch(const std::out_of_range& oor){
                        std::cerr << "Port out of range" << std::endl;
                        port =80;
                    }
                    //Verify port is within valid range
                    if (port < MINIMUM_VALID_PORT || port > MAXIMUM_VALID_PORT){
                        std::cerr << "Port out of range" << std::endl;
                        port =80; 
                    }

                }
            }
        }
    }
    return port;
}
