#include "request_handler.h"
#include <boost/asio.hpp>
#include <boost/beast/http.hpp>


/*RequestHandler::RequestHandler(short bytes_transferred, const char data[], ServerPaths server_paths)
    :server_paths_(server_paths), file_path_("")
{
    // Create a request parser
    // boost::beast::http::request_parser<boost::beast::http::string_body> parser;
    auto buffer = boost::asio::buffer(data, bytes_transferred);
    // Parse the buffer
    boost::beast::error_code ec;
    this->parser.put(buffer, ec);
    this->bytes_transferred = bytes_transferred;
    this->data = data;

}*/

RequestHandler::RequestHandler() {}
