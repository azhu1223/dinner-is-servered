#include <iostream>
#include "server.h"
#include "config_parser.h"
#include "config_interpreter.h"

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

        server s(io_service, getPort(config));

        s.run();
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
        return 1;
    }
    return 0;
}
