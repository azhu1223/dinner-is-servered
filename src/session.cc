#include "session.h"
#include <string>
#include <vector>
#include <boost/bind.hpp>

session::session(boost::asio::io_service& io_service)
    : socket_(io_service) {
}

boost::asio::ip::tcp::socket& session::socket() {
    return socket_;
}

void session::start() {
    socket_.async_read_some(boost::asio::buffer(data_, max_length),
        boost::bind(&session::handle_read, this,
          boost::asio::placeholders::error,
          boost::asio::placeholders::bytes_transferred));
}

void session::handle_read(const boost::system::error_code& error, size_t bytes_transferred) {
    if (!error) {
        std::vector<char> response = create_response(bytes_transferred);

        boost::asio::async_write(socket_,
            boost::asio::buffer(response.data(), response.size()),
            boost::bind(&session::handle_write, this,
              boost::asio::placeholders::error));
    } else {
        delete this;
    }
}

void session::handle_write(const boost::system::error_code& error) {
    if (!error) {
        socket_.async_read_some(boost::asio::buffer(data_, max_length),
            boost::bind(&session::handle_read, this,
              boost::asio::placeholders::error,
              boost::asio::placeholders::bytes_transferred));
    } else {
        delete this;
    }
}

std::vector<char> session::create_response(short bytes_transferred) {
    const std::string status = "HTTP/1.0 200 OK\r\n";
    const std::string content_type = "Content-Type: text/plain\r\n";
    const std::string content_length_text = "Content-Length: " + std::to_string(bytes_transferred) + "\r\n\r\n";

    // Constructs the header
    const std::string header = status + content_type + content_length_text;

    std::vector<char> response(header.begin(), header.end());

    // Adds the GET request sent to the server as the contents of the reponse
    response.insert(response.end(), &data_[0], &data_[bytes_transferred]);

    return response;
}
