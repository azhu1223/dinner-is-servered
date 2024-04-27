#include <iostream>
#include "server.h"
#include "config_parser.h"
#include "config_interpreter.h"
#include "logger.h"
#include <boost/log/trivial.hpp>
#include "signal_handler.h"


int main(int argc, char* argv[]) {
    try {
        if ( !(argc ==2 || (argc==3 && std::string(argv[2]) == "--disable-file-logging") ) ) {
            BOOST_LOG_TRIVIAL(fatal) << "Usage: server <config_file> [--disable-file-logging]";  // Corrected usage message
            return 1;
        }

        //Initialize logging
        bool enable_file_logging = true;
        // Check for "--disable-file-logging" flag
        if (argc == 3 && std::string(argv[2]) == "--disable-file-logging") {
            enable_file_logging = false;
        }
        intialize_logging(enable_file_logging);

        //Intialize termination signal handler
        signal (SIGINT, sig_handler);
        
        boost::asio::io_service io_service;

        NginxConfig config;
        NginxConfigParser config_parser = NginxConfigParser();
        bool successfulConfigParse = config_parser.Parse(argv[1], &config);
        if (!successfulConfigParse) {
            BOOST_LOG_TRIVIAL(fatal) << "Error in parsing the Nginx config file\n";
            return 1;
        }

        server s(io_service, getPort(config), getServerPaths(config));

        s.run();
    } catch (const std::exception& e) {
        BOOST_LOG_TRIVIAL(fatal) << "Exception: " << e.what();
        return 1;
    }
    return 0;
}
