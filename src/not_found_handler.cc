#include "not_found_handler.h"
#include "request_handler.h"
#include <string>
#include <boost/log/trivial.hpp>


NotFoundHandler::NotFoundHandler(short bytes_transferred, const char data[], ServerPaths server_paths) : RequestHandler(bytes_transferred, data, server_paths) {}

std::vector<char> NotFoundHandler::create_response() {
    BOOST_LOG_TRIVIAL(error) << "404 not found: ";
    std::string response = "HTTP/1.1 404 Not Found\r\n"
                           "Content-Type: text/plain\r\n"
                           "Content-Length: 17\r\n\r\n"
                           "404 Not Found\r\n\r\n";
    return std::vector<char>(response.begin(), response.end());
}
