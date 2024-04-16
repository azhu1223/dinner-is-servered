#include <iostream>
#include "server.h"
#include "config_parser.h"

int main(int argc, char* argv[]) {
    try {
        if (argc != 2) {
            std::cerr << "Usage: server <config_file>\n";  // Corrected usage message
            return 1;
        }
        
        boost::asio::io_service io_service;

        NginxConfig config;
        NginxConfigParser config_parser = NginxConfigParser();
        bool successfulConfigParse = config_parser.Parse(argv[1], &config);
        if (!successfulConfigParse) {
            std::cerr << "Error in parsing the Nginx config file\n";
            return 1;
        }

        // Default port is 80
        int port = 80;

        for (const auto& statement : config.statements_) {
            // Find the server statement
            if (statement->tokens_[0] == "server") {
                for (const auto& nestedStatement : statement->child_block_->statements_) {
                    // Find the listen statement
                    if (nestedStatement->tokens_[0] == "listen") {
                        port = std::stoi(nestedStatement->tokens_[1]);
                    }
                }
            }
        }

        std::cout << "Port is " << port << std::endl;
        server s(io_service, port);
        s.run();
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
        return 1;
    }
    return 0;
}
