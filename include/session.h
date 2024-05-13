#ifndef SESSION_H
#define SESSION_H

#include "utils.h"
#include <boost/asio.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <string>
#include <vector>

namespace http = boost::beast::http;

class session {
public:
    session(boost::asio::io_service& io_service, ServerPaths server_paths);
    bool start();
    boost::asio::ip::tcp::socket& socket();
    bool handle_read(const boost::system::error_code& error, size_t bytes_transferred);
    bool handle_write(const boost::system::error_code& error);

private:
    ServerPaths server_paths_;
    boost::asio::ip::tcp::socket socket_;
    enum { max_length = 1024 };
    char data_[max_length];
    boost::beast::flat_buffer buff_;
    http::response<http::vector_body<char>> response_;
    http::request<http::vector_body<char>> request_;
};
#endif // SESSION_H